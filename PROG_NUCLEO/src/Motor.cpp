#include "Motor.hpp"

Motor::Motor() {
	modeTest = false;
	interfaceCom = new UIcom;
	refreshUI = new Ticker;
	instEncoder = new Encoders;
	routineAsserv = new Ticker;
	motorD = new PwmOut(PIN_PWMD);
	motorG = new PwmOut(PIN_PWMG);
	motorD->period_us(PERIODE_PWM);
	motorG->period_us(PERIODE_PWM);
	motorD->write(0);
	motorG->write(0);
	sens_mD = new DigitalOut(PIN_SENSMD);
	sens_mG = new DigitalOut(PIN_SENSMG);
	for (int i = 0; i < TAILLE_TABLEAUX; i++) {
		pwms_d[i] = 0;
		pwms_g[i] = 0;
		errors_d[i] = 0;
		errors_g[i] = 0;
	}
}

Motor::Motor(Serial *pc_out) //Ajouter les pins dans les param�tres de constructeur
{
	pc = pc_out;
	buff_in = "";
	modeTest = true;
	instEncoder = new Encoders(pc);
	routineAsserv = new Ticker;
	affichage_debug = new Timer;
	affichage_debug->start();
	motorD = new PwmOut(PIN_PWMD);
	motorG = new PwmOut(PIN_PWMG);
	motorD->period_us(PERIODE_PWM);
	motorG->period_us(PERIODE_PWM);
	motorD->write(0);
	motorG->write(0);
	sens_mD = new DigitalOut(PIN_SENSMD);
	sens_mG = new DigitalOut(PIN_SENSMG);
	bouton = new DigitalIn(USER_BUTTON);
	for (int i = 0; i < TAILLE_TABLEAUX; i++) {
		pwms_d[i] = 0;
		pwms_g[i] = 0;
		errors_d[i] = 0;
		errors_g[i] = 0;
	}
}

void Motor::asserv_vitesse(float vitesse) {
	consigne_vitesse = 0.05; //((vitesse / PERIMETER) * RESOLUTION)/INC_MAX;
	routineAsserv->attach(callback(this, &Motor::routine), PERIODE_ASSERV);
	if(!modeTest)
		refreshUI->attach(callback(this, &Motor::send_to_ui), PERIODE_SEND_UI);
	while (1) {
		if (modeTest)
			debug();
	}
}

void Motor::asserv_position(float distance) {
	consigne_position = (distance / PERIMETER) * RESOLUTION;
	calc_sens(consigne_position, consigne_position);
}

void Motor::asserv_angle(float angle){
	float envergure = 300; // In mm
	float distance_roue = (angle * 3.14 / 180)*envergure;
	float consigne_angle = (distance_roue / PERIMETER) * RESOLUTION;
	calc_sens(consigne_angle, -consigne_angle); //Angle > 0 Rotate counterclockwise
}

void Motor::routine() {
	static float coeffX[TAILLE_TABLEAUX] = { 0.002996, 0.005659, 0.003105, 0.0004418 };
	static float coeffY[TAILLE_TABLEAUX] = { 0.9471, -0.9995, -0.9477, 0 };

	calc_vitesse(); //Calcul de vitesse_d et vitesse_g

	float epsilon_d = consigne_vitesse - vitesse_d;
	float epsilon_g = consigne_vitesse - vitesse_g;

	push_in_tab(epsilon_d, errors_d);
	push_in_tab(epsilon_g, errors_g);


	pwmd = coeffX[0] * errors_d[0] + coeffX[1] * errors_d[1] + coeffX[2] * errors_d[2] + coeffX[3] * errors_d[3]
		- (coeffY[0] * pwms_d[0] + coeffY[1] * pwms_d[1] + coeffY[2] * pwms_d[2]);
	//pc->printf("%f\n", pwmd);
	pwmg = coeffX[0] * errors_g[0] + coeffX[1] * errors_g[1] + coeffX[2] * errors_g[2] + coeffX[3] * errors_g[3]
		- (coeffY[0] * pwms_g[0] + coeffY[1] * pwms_g[1] + coeffY[2] * pwms_g[2]);

	pwmd = (pwmd < 0) ? 0 : pwmd;
	pwmd = (pwmd > 1) ? 1 : pwmd;
	pwmg = (pwmg < 0) ? 0 : pwmg;
	pwmg = (pwmg > 1) ? 1 : pwmg;

	push_in_tab(pwmd, pwms_d);
	push_in_tab(pwmg, pwms_g);

	motorD->write(pwmd);
	motorG->write(pwmg);
}

void Motor::calc_sens(float vd, float vg)
{
	//Right motor direction
	if (vd >= 0) {
		sensD = 1;
		sens_mD->write(1);
	}
	else {
		sensD = 0;
		sens_mD->write(0);
	}

	//Left motor direction
	if (vg >= 0) {
		sensG = 1;
		sens_mG->write(1);
	}
	else {
		sensG = 0;
		sens_mG->write(0);
	}
}

void Motor::calc_vitesse() {
	int imp_d_act = instEncoder->getImpEncD();
	int imp_g_act = instEncoder->getImpEncG();

	vitesse_d = (sens_mD->read() * 2 - 1)*((imp_d_act) / PERIODE_ASSERV)/(INC_MAX);
	vitesse_g = (sens_mG->read() * 2 - 1)*((imp_g_act) / PERIODE_ASSERV)/(INC_MAX);
}

void Motor::debug()
{
	if (affichage_debug->read_ms() >= 1000) {
		//pc->printf("PWMD : %f PWMG : %f\n", pwmd, pwmg);
		//pc->printf("Imp G: %d\nImp D : %d\n", instEncoder->getImpEncG(), instEncoder->getImpEncD());
		//pc->printf("e G : %f e D : %f\n", consigne_vitesse - vitesse_g, consigne_vitesse - vitesse_d);
		//pc->printf("corr G : %f corr D : %f\n", (consigne_vitesse - vitesse_g)*KP, (consigne_vitesse - vitesse_d)*KP);
		//pc->printf("Vitesse G : %f Vitesse D : %f\n", vitesse_g * PERIMETER / RESOLUTION, vitesse_d * PERIMETER / RESOLUTION);
		//pc->printf("Vitesse G : %f inc/s\nVitesse D : %f inc/s\n\n", vitesse_d, vitesse_g);
		//pc->printf("Inc G : %d\nInc D : %d\n\n", instEncoder->getImpEncG(), instEncoder->getImpEncD());
		affichage_debug->reset();
	}
	if (pc->readable()) {
		char c = pc->getc();
		if (c == '\n') {
			consigne_vitesse = stof(buff_in);
			pc->printf("Consigne chang�e � : %f", consigne_vitesse);
			buff_in.clear();
		}
		else
			buff_in += c;
	}
}

void Motor::push_in_tab(float x, float tableau[]){
	for (int i = 0; i < TAILLE_TABLEAUX-1; i++) {
		tableau[TAILLE_TABLEAUX - i - 1] = tableau[TAILLE_TABLEAUX - i - 2];
	}
	tableau[0] = x;
}

void Motor::send_to_ui() {
	interfaceCom->set_bat18(0.);
	interfaceCom->set_bat9(1.);
	interfaceCom->set_current_d(2.);
	interfaceCom->set_current_g(3.);
	interfaceCom->set_dist_d(4.);
	interfaceCom->set_dist_g(5.);
	interfaceCom->set_erreur_d(errors_d[0]);
	interfaceCom->set_erreur_g(errors_g[0]);
	interfaceCom->set_pwmd(pwmd);
	interfaceCom->set_pwmg(pwmg);
}

Motor::~Motor()
{
}
