#include "Motor.hpp"

Motor::Motor() {
	modeTest = false;
	instEncoder = new Encoders;
	routineAsserv = new Ticker;
	motorD = new PwmOut(PIN_PWMD);
	motorG = new PwmOut(PIN_PWMG);
	motorD->period_us(PERIODE_PWM);
	motorG->period_us(PERIODE_PWM);
	sens_mD = new DigitalOut(PIN_SENSMD);
	sens_mG = new DigitalOut(PIN_SENSMG);
	dist_d = 0;
	dist_g = 0;
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
	sens_mD = new DigitalOut(PIN_SENSMD);
	sens_mG = new DigitalOut(PIN_SENSMG);
	bouton = new DigitalIn(USER_BUTTON);
	dist_d = 0;
	dist_g = 0;
}

//Vitesse en mm/s
void Motor::asserv_vitesse(float vitesse) {
	consigne_vitesse = (vitesse / PERIMETER) * RESOLUTION;
	routineAsserv->attach(callback(this, &Motor::routine), PERIODE_ASSERV); //256Hz
	calc_sens(consigne_vitesse, consigne_vitesse);
	while (1) {
		if (modeTest)
			test_encodeurs();
	}
}

void Motor::routine()
{
	calc_vitesse();
	float epsilon_d = consigne_vitesse - vitesse_d;
	float epsilon_g = consigne_vitesse - vitesse_g;
	float delta_ed = epsilon_dant - epsilon_d;
	float delta_eg = epsilon_gant - epsilon_g;
	sum_epsilon_d += epsilon_d;
	sum_epsilon_g += epsilon_g;
	epsilon_dant = epsilon_d;
	epsilon_gant = epsilon_g;
	pwmd_eff = (epsilon_d * KP) / VITESSE_MAX;
	pwmg_eff = (epsilon_g * KP) / VITESSE_MAX;

	pwmd_eff = (pwmd_eff < 0) ? 0 : pwmd_eff;
	pwmd_eff = (pwmd_eff > 1) ? 1 : pwmd_eff;
	pwmg_eff = (pwmg_eff < 0) ? 0 : pwmg_eff;
	pwmg_eff = (pwmg_eff > 1) ? 1 : pwmg_eff;

	motorD->write(pwmd_eff);
	motorG->write(pwmg_eff);
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
			//pc->printf("Imp G: %d\nImp D : %d\n", instEncoder->getImpEncG(), instEncoder->getImpEncD());
			pc->printf("e G : %f e D : %f\n", consigne_vitesse - vitesse_g, consigne_vitesse - vitesse_d);
			//pc->printf("corr G : %f corr D : %f\n", (consigne_vitesse - vitesse_g)*KP, (consigne_vitesse - vitesse_d)*KP);
			pc->printf("Vitesse G : %f Vitesse D : %f\n", vitesse_g * PERIMETER / RESOLUTION, vitesse_d * PERIMETER / RESOLUTION);
			//pc->printf("Vitesse G : %f inc/s\nVitesse D : %f inc/s\n\n", vitesse_d, vitesse_g);
			pc->printf("Inc G : %d\nInc D : %d\n\n", instEncoder->getImpEncG(), instEncoder->getImpEncD());
			affichage_debug->reset();
		}
	}
}

Motor::~Motor()
{
}
