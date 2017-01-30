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
	consigne_vitesse = 0;
	consigne_vd_change(0.);
	consigne_vd_change(0.);
	dist_d = 0;
	dist_g = 0;
}

Motor::Motor(Serial *pc_out) //Ajouter les pins dans les paramètres de constructeur
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
	consigne_vitesse = 0;
	consigne_vd_change(0.);
	consigne_vd_change(0.);
	dist_d = 0;
	dist_g = 0;
}

void Motor::vitesse(float vitesse) {
	int reading = 0;
	consigne_vitesse = 0;
	consigne_vd_change(0.);
	consigne_vd_change(0.);
	routineAsserv->attach(callback(this, &Motor::asserv_vitesse), PERIODE_ASSERV);
	if (!modeTest)
		refreshUI->attach(callback(this, &Motor::send_to_ui), PERIODE_SEND_UI);
	else
		pc->printf("Passage en mode test.\n");
	while (1) {
		if (modeTest)
		{
			if (pc->readable() && !reading)
			{
				reading = 1;
				debug(&reading);
			}
		}
	}
}

void Motor::trajectoire(float vitesse) {
	int reading = 0;
	consigne_vitesse = 0;
	consigne_vd_change(0.);
	consigne_vd_change(0.);
	routineAsserv->attach(callback(this, &Motor::asserv_trajectoire), PERIODE_ASSERV);
	if (!modeTest)
		refreshUI->attach(callback(this, &Motor::send_to_ui), PERIODE_SEND_UI);
	else
		pc->printf("Passage en mode test.\n");
	while (1) {
		if (modeTest)
		{
			if (pc->readable() && !reading)
			{
				reading = 1;
				debug(&reading);
			}
		}
	}
}

void Motor::position(float distance, float vitesse) {
	consigne_position = (distance / PERIMETER) * RESOLUTION;
	calc_sens(consigne_position, consigne_position);
}

void Motor::angle(float angle){
	float envergure = 300; // In mm
	float distance_roue = (angle * 3.14 / 180)*envergure;
	float consigne_angle = (distance_roue / PERIMETER) * RESOLUTION;
	calc_sens(consigne_angle, -consigne_angle); //Angle > 0 Rotate counterclockwise
}

void Motor::asserv_position() {

}

void Motor::asserv_vitesse() {
	static float coeffX[TAILLE_TABLEAUX] = { 0.002996, 0.005659, 0.003105, 0.0004418 };
	static float coeffY[TAILLE_TABLEAUX] = { 0.9471, -0.9995, -0.9477, 0 };

	calc_vitesse(); //Calcul de vitesse_d et vitesse_g

	float epsilon_d = consigne_vitesse_d - vitesse_d;
	float epsilon_g = consigne_vitesse_g - vitesse_g;
	push_in_tab(epsilon_d, errors_d);
	push_in_tab(epsilon_g, errors_g);


	pwmd = coeffX[0] * errors_d[0] + coeffX[1] * errors_d[1] + coeffX[2] * errors_d[2] + coeffX[3] * errors_d[3]
		- (coeffY[0] * pwms_d[0] + coeffY[1] * pwms_d[1] + coeffY[2] * pwms_d[2]);
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

void Motor::asserv_trajectoire() {
	//calc_vitesse();
	static float coeff = 0.000005; //0.00000244140625;
	float epsilon = dist_d - dist_g;
	consigne_vitesse_d = consigne_vitesse - epsilon*coeff;
	consigne_vitesse_g = consigne_vitesse + epsilon*coeff;

	asserv_vitesse();
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

	dist_d += imp_d_act;
	dist_g += imp_g_act;
}

void Motor::debug(int* reading)
{
/*	if (affichage_debug->read_ms() >= 1000) {
		//pc->printf("PWMD : %f PWMG : %f\n", pwmd, pwmg);
		//pc->printf("Imp G: %d\nImp D : %d\n", instEncoder->getImpEncG(), instEncoder->getImpEncD());
		//pc->printf("e G : %f e D : %f\n", consigne_vitesse - vitesse_g, consigne_vitesse - vitesse_d);
		//pc->printf("corr G : %f corr D : %f\n", (consigne_vitesse - vitesse_g)*KP, (consigne_vitesse - vitesse_d)*KP);
		//pc->printf("Vitesse G : %f Vitesse D : %f\n", vitesse_g * PERIMETER / RESOLUTION, vitesse_d * PERIMETER / RESOLUTION);
		//pc->printf("Vitesse G : %f inc/s\nVitesse D : %f inc/s\n\n", vitesse_d, vitesse_g);
		//pc->printf("Inc G : %d\nInc D : %d\n\n", instEncoder->getImpEncG(), instEncoder->getImpEncD());
		affichage_debug->reset();
	}*/
	static char test[64] = "";
	static char c;
	while (pc->readable())
	{
		pc->gets(test, 5);
	}
	pc->printf("%s\n", test);
	float result = atof(test);
	dist_d = 0;
	dist_g = 0;
	if (result < 0) {
		calc_sens(-1, -1);
		consigne_vitesse = -result;
		consigne_vd_change(-result);
		consigne_vg_change(-result);
	}
	else {
		calc_sens(1, 1);
		consigne_vitesse = result;
		consigne_vd_change(result);
		consigne_vg_change(result);
	}
	pc->printf("Consigne changée à : %f\n", consigne_vitesse);
	for (int j = 0; j < 64; j++)
		test[j] = 0;
	*reading = 0;
}

void Motor::consigne_vd_change(float consigne) {
	consigne_vitesse_d = consigne;
	for (int i = 0; i < TAILLE_TABLEAUX; i++) {
		push_in_tab(0., errors_d);
		push_in_tab(consigne_vitesse_d, pwms_d);
	}
}

void Motor::consigne_vg_change(float consigne) {
	consigne_vitesse_g = consigne;
	for (int i = 0; i < TAILLE_TABLEAUX; i++) {
		push_in_tab(0., errors_g);
		push_in_tab(consigne_vitesse_g, pwms_g);
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
