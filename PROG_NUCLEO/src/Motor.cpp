#include "Motor.hpp"

Motor::Motor() {
	modeTest = false;
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
	for (int i = 0; i < 9; i++) {
		vs_d[i] = 0;
		vs_g[i] = 0;
		errors_d[i] = 0;
		errors_g[i] = 0;
	}
}

Motor::Motor(Serial *pc_out) //Ajouter les pins dans les paramètres de constructeur
{
	pc = pc_out;
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
	for (int i = 0; i < 9; i++) {
		vs_d[i] = 0;
		vs_g[i] = 0;
		errors_d[i] = 0;
		errors_g[i] = 0;
	}
}

void Motor::asserv_vitesse(float vitesse) {
	consigne_vitesse = (vitesse / PERIMETER) * RESOLUTION;
	routineAsserv->attach(callback(this, &Motor::routine), PERIODE_ASSERV); //256Hz
	while (1) {
		if (modeTest)
			test_encodeurs();
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

void Motor::routine()
{
	float coeffX[8] = { 0.3143, 1.813, 4.352, 5.57, 4.014, 1.55, 0.2561, 0.002982 };
	float coeffY[8] = { 0.3036, 1.906, 4.982, 6.953, 5.475, 2.322, 0.4277, 0.007978 };

	calc_vitesse();

	push_in_tab(vitesse_d, vs_d);
	push_in_tab(vitesse_g, vs_g);

	float epsilon_d = consigne_vitesse - vitesse_d;
	float epsilon_g = consigne_vitesse - vitesse_g;

	for (int i = 0; i < 8; i++) {
		pwmd += errors_d[i] * coeffX[i] / VITESSE_MAX;
		pwmg += errors_g[i] * coeffX[i] / VITESSE_MAX;
		pwmd -= vs_d[i] * coeffY[i] / VITESSE_MAX;
		pwmg -= vs_g[i] * coeffY[i] / VITESSE_MAX;
	}

	pwmd = (pwmd < 0) ? 0 : pwmd;
	pwmd = (pwmd > 1) ? 1 : pwmd;
	pwmg = (pwmg < 0) ? 0 : pwmg;
	pwmg = (pwmg > 1) ? 1 : pwmg;

	push_in_tab(epsilon_d, errors_d);
	push_in_tab(epsilon_g, errors_g);

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

	vitesse_d = (sens_mD->read() * 2 - 1)*((imp_d_act - imp_d) / PERIODE_ASSERV);
	vitesse_g = (sens_mG->read() * 2 - 1)*((imp_g_act - imp_g) / PERIODE_ASSERV);

	imp_d = imp_d_act;
	imp_g = imp_g_act;
}

void Motor::test_encodeurs()
{
	pc->printf("Test des encodeurs.\n");
	while (bouton->read() == 1);
	while (bouton->read() == 0) {
		if (affichage_debug->read_ms() >= 1000) {
			pc->printf("PWMD : %f PWMG : %f\n", pwmd, pwmg);
			//pc->printf("Imp G: %d\nImp D : %d\n", instEncoder->getImpEncG(), instEncoder->getImpEncD());
			pc->printf("e G : %f e D : %f\n", consigne_vitesse - vitesse_g, consigne_vitesse - vitesse_d);
			//pc->printf("corr G : %f corr D : %f\n", (consigne_vitesse - vitesse_g)*KP, (consigne_vitesse - vitesse_d)*KP);
			pc->printf("Vitesse G : %f Vitesse D : %f\n", vitesse_g * PERIMETER / RESOLUTION, vitesse_d * PERIMETER / RESOLUTION);
			//pc->printf("Vitesse G : %f inc/s\nVitesse D : %f inc/s\n\n", vitesse_d, vitesse_g);
			//pc->printf("Inc G : %d\nInc D : %d\n\n", instEncoder->getImpEncG(), instEncoder->getImpEncD());
			affichage_debug->reset();
		}
	}
}

void Motor::push_in_tab(float x, float tableau[]){
	for (int i = 0; i < 8-1; i++) {
		tableau[8 - i - 1] = tableau[8 - i - 2];
	}
	tableau[0] = x;
}

Motor::~Motor()
{
}
