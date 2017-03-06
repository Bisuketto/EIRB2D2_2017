#include "Motor.hpp"

Motor::Motor() {
	modeTest = false;
	status = false;
	hasToStop = false;
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
	tc = new Timer;
	tc->start();
}

Motor::Motor(Serial *pc_out) //Ajouter les pins dans les paramètres de constructeur
{
	pc = pc_out;
	buff_in = "";
	modeTest = true;
	status = false;
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
	tc = new Timer;
	tc->start();
}

void Motor::link_to_ui(UIcom * ui){
	if (!modeTest) {
		interfaceCom = ui;
		refreshUI->attach(callback(this, &Motor::send_to_ui), PERIODE_SEND_UI);
	}
}

void Motor::stop(){
	routineAsserv->detach();
	motorD->write(0);
	motorG->write(0);
	status = false;
	hasToStop = false;
}

void Motor::vitesse(float vitesse) {
	int reading = 0;
	consigne_vitesse = 0;
	consigne_vd_change(0.);
	consigne_vg_change(0.);
	routineAsserv->attach(callback(this, &Motor::asserv_vitesse), PERIODE_ASSERV);
	if (modeTest) {
		pc->printf("Mode Test Vitesse\n");
	}
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

void Motor::vitesse_man(float vg, float vd) {
	if (vg > 0) {
		sens_mG->write(1);
	}
	else {
		sens_mG->write(0);
		vg *= -1;
	}
	if (vd > 0) {
		sens_mD->write(1);
	}
	else {
		sens_mD->write(0);
		vd *= -1;
	}
	motorD->write(vd);
	motorG->write(vg);
	routineAsserv->detach();
	status = false;
}

void Motor::trajectoire(float vitesse) {
	int reading = 0;
	consigne_vitesse = 0;
	consigne_vd_change(0.);
	consigne_vg_change(0.);
	routineAsserv->detach();
	routineAsserv->attach(callback(this, &Motor::asserv_trajectoire), PERIODE_ASSERV);
	if (modeTest) {
		pc->printf("Mode Test Trajectoire\n");
	}
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

void Motor::position(float distance, float angle) {
	int reading = 0;
	consigne_a_change(angle);
	consigne_p_change(distance);
	tc->reset();
	routineAsserv->detach();
	routineAsserv->attach(callback(this, &Motor::asserv_position), PERIODE_ASSERV);
	if (modeTest) {
		pc->printf("Mode Test de Positionnement\n");
	}
	status = true;
}

void Motor::angle(float angle){
	float envergure = 360; // In mm
	float distance_roue = (angle * 3.14 / 180)*envergure;
	float consigne_angle = (distance_roue / PERIMETER) * RESOLUTION;
	calc_sens(consigne_angle, -consigne_angle); //Angle > 0 Rotate counterclockwise
}

void Motor::asserv_position() {
	static float coeffXP[TAILLE_TABLEAUX] = { 0.07259, -0.06858, -0.07254, 0.06863 };// { 0.03564, -0.03161, -0.03561, 0.03165 }Ok;
	static float coeffYP[TAILLE_TABLEAUX] = { -1.334, -0.1061, 0.4404, 0 };// { 0.6591, -0.9903, 0.5688, 0 }Ok;
	static float coeffXA[TAILLE_TABLEAUX] = { 0.0363, -0.03429, -0.03627, 0.03432 };
	static float coeffYA[TAILLE_TABLEAUX] = { -1.334, -0.1061, 0.4404, 0 };
	//static float coeff = 0.00001;//0.000005;

	calc_vitesse();

	float dist_from_start = (dist_d + dist_g) / 2.;

	float epsilon_angle = consigne_angle - (dist_d - dist_g);
	float epsilon_dist = consigne_hyperbolique() - dist_from_start;

	//Calcul de la pwm distance
	push_in_tab(epsilon_dist, errors_dist);
	float vitesse = coeffXP[0] * errors_dist[0] + coeffXP[1] * errors_dist[1] + coeffXP[2] * errors_dist[2] + coeffXP[3] * errors_dist[3]
		- (coeffYP[0] * consignes_v[0] + coeffYP[1] * consignes_v[1] + coeffYP[2] * consignes_v[2]);
	push_in_tab(vitesse, consignes_v);
	//Calcul de la pwm angle
	push_in_tab(epsilon_angle, errors_angle);
	float angle = coeffXA[0] * errors_angle[0] + coeffXA[1] * errors_angle[1] + coeffXA[2] * errors_angle[2] + coeffXA[3] * errors_angle[3]
		-(coeffYA[0] * consignes_a[0] + coeffYA[1] * consignes_a[1] + coeffYA[2] * consignes_a[2]);
	push_in_tab(angle, consignes_a);

	float cd = vitesse + angle;
	float cg = vitesse - angle;
	//Normalisation
	cd = cd*18*2/INC_MAX+0.05;
	cg = cg*18*2/INC_MAX+0.05;

	if (cd < 0) {
		sens_mD->write(0);
		cd = -1 *cd;
	}
	else {
		sens_mD->write(1);
	}
	if (cg < 0) {
		sens_mG->write(0);
		cg = -1 * cg;
	}
	else {
		sens_mG->write(1);
	}
	
	//Saturations
	
	cd = (cd > 1) ? 1 : cd;
	cd = (cd < 0) ? 0 : cd;
	cg = (cg > 1) ? 1 : cg;
	cg = (cg < 0) ? 0 : cg;
	

	pwmd = cd;
	pwmg = cg;	

	motorD->write(cd);
	motorG->write(cg);

	if (tc->read_us() - last_change_d > TIMEOUT_ASSERVPOS) {
		interfaceCom->sendText("Fin du processus d'asserv en position\n");
		stop();
	}
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

	pwmd = (pwmd < -1) ? -1 : pwmd;
	pwmd = (pwmd > 1) ? 1 : pwmd;
	pwmg = (pwmg < -1) ? -1 : pwmg;
	pwmg = (pwmg > 1) ? 1 : pwmg;
	push_in_tab(pwmd, pwms_d);
	push_in_tab(pwmg, pwms_g);

	calc_sens(pwmd, pwmg);
	pwmd *= (pwmd > 0) * 2 - 1;
	pwmg *= (pwmg > 0) * 2 - 1;

	motorD->write(pwmd);
	motorG->write(pwmg);
}

void Motor::asserv_trajectoire(){
	//calc_vitesse();
	static float coeff = 0.000005; //0.00000244140625;
	float epsilon = dist_d - dist_g;
	consigne_vitesse_d = consigne_vitesse - epsilon*coeff;
	consigne_vitesse_g = consigne_vitesse + epsilon*coeff;

	asserv_vitesse();
}

float Motor::consigne_hyperbolique() {
	float t = tc->read_us()*0.000001;
	float acceleration = ACCELERATION;
	float vmax = VITESSEMAX;
	
	if (consigne_position < 0) {
		acceleration *= -1;
		vmax *= -1;
	}

	float omega = consigne_position / vmax + vmax / acceleration;
	float alpha = vmax / acceleration;
	float beta = omega - vmax / acceleration;
	float gamma;
	
	if (alpha < beta) {
		if (t < alpha) {
			return 0.5*acceleration*t*t;
		}
		else if (t < beta) {
			return 0.5*acceleration*alpha*alpha + vmax*(t - alpha);
		}
		else if (t < omega) {
			return 0.5*acceleration*alpha*alpha + vmax*(beta - alpha) + vmax*(t - beta) - 0.5*acceleration*(t - beta)*(t - beta);
		}
		else {
			return consigne_position;
		}
	}
	else {
		omega = 2 * sqrt(consigne_position / acceleration);
		gamma = omega / 2;
		if (t < gamma) {
			return 0.5*acceleration*t*t;
		}
		else if (t < omega) {
			return 0.5*acceleration*gamma*gamma + (acceleration*gamma)*(t - gamma) - 0.5*acceleration*(t - gamma)*(t - gamma);
		}
		else {
			return consigne_position;
		}
	}
	return 0;
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

	vitesse_d = (sens_mD->read() * 2 - 1)*((imp_d_act) / PERIODE_ASSERV) / (INC_MAX);
	vitesse_g = (sens_mG->read() * 2 - 1)*((imp_g_act) / PERIODE_ASSERV) / (INC_MAX);

	if (imp_d_act != 0 || imp_d_act != 0)
		last_change_d = tc->read_us();

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
		pc->gets(test, 7);
	}
	pc->printf("%s\n", test);
	float result = atof(test);
	
	consigne_angle = 0;
	dist_d = 0;
	dist_g = 0;
	/*
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
	*/
	consigne_position = result;
	pc->printf("Consigne changée à : %f\n", consigne_position);
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

void Motor::consigne_a_change(float consigne) {
	consigne_angle = consigne;
	for (int i = 0; i < TAILLE_TABLEAUX; i++) {
		push_in_tab(0., consignes_a);
		push_in_tab(0., errors_angle);
	}
}

void Motor::consigne_p_change(float consigne) {
	consigne_position = consigne;
	dist_d = 0;
	dist_g = 0;
	for (int i = 0; i < TAILLE_TABLEAUX; i++) {
		push_in_tab(0., errors_dist);
		push_in_tab(0., consignes_v);
	}
}

void Motor::push_in_tab(float x, float tableau[]){
	for (int i = 0; i < TAILLE_TABLEAUX-1; i++) {
		tableau[TAILLE_TABLEAUX - i - 1] = tableau[TAILLE_TABLEAUX - i - 2];
	}
	tableau[0] = x;
}

void Motor::send_to_ui() {
	float t = tc->read_us()*0.000001;
	float omega = consigne_position / VITESSEMAX + VITESSEMAX / ACCELERATION;
	float alpha = VITESSEMAX / ACCELERATION;
	float beta = omega - VITESSEMAX / ACCELERATION;
	interfaceCom->set_bat18(t);
	interfaceCom->set_bat9(omega);
	interfaceCom->set_current_d(alpha);
	interfaceCom->set_current_g(beta);
	interfaceCom->set_dist_d(consigne_hyperbolique());
	interfaceCom->set_dist_g(dist_g);
	interfaceCom->set_erreur_d(consigne_position - ((dist_d + dist_g) / 2.));
	interfaceCom->set_erreur_g(dist_d - dist_g);
	interfaceCom->set_pwmd(pwmd);
	interfaceCom->set_pwmg(pwmg);
	interfaceCom->set_status_motor(status);
}

bool Motor::get_state() {
	return status;
}

Motor::~Motor()
{
}
