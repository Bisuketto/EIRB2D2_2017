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
	calc_vitesse();
	float epsilon_d = consigne_vitesse - vitesse_d;
	float epsilon_g = consigne_vitesse - vitesse_g;
	float delta_ed = epsilon_v_dant - epsilon_d;
	float delta_eg = epsilon_v_gant - epsilon_g;
	sum_epsilon_v_d += epsilon_d;
	sum_epsilon_v_g += epsilon_g;
	epsilon_v_dant = epsilon_d;
	epsilon_v_gant = epsilon_g;
	pwmd = (epsilon_d * KP + delta_ed * KD + sum_epsilon_v_d * KI) / VITESSE_MAX;
	pwmg = (epsilon_g * KP + delta_eg * KD + sum_epsilon_v_g * KI) / VITESSE_MAX;

	pwmd = (pwmd < 0) ? 0 : pwmd;
	pwmd = (pwmd > 1) ? 1 : pwmd;
	pwmg = (pwmg < 0) ? 0 : pwmg;
	pwmg = (pwmg > 1) ? 1 : pwmg;

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

Motor::~Motor()
{
}
