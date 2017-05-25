#include "Motor.hpp"

Motor::Motor() {
	modeTest = false;
	status = false;
	hasToStop = false;
	refreshUI = new Ticker;
	//instEncoder = new Encoders;
	routineAsserv = new Ticker;
	motorD = new PwmOut(PIN_PWMD);
	motorG = new PwmOut(PIN_PWMG);
	motorD->period_us(PERIODE_PWM);
	motorG->period_us(PERIODE_PWM);
	motorD->write(0);
	motorG->write(0);
	sens_mD = new DigitalOut(PIN_SENSMD);
	sens_mG = new DigitalOut(PIN_SENSMG);
	led = new DigitalOut(LED1);
	consigne_vitesse = 0;
	consigne_vd_change(0.);
	consigne_vd_change(0.);
	dist_d = 0;
	dist_g = 0;
	tc = new Timer;
	tirette = new Timer;
	tirette->start();
	tc->start();
	gp2_front_r = new AnalogIn(GP2_FRONTR);
	gp2_front_l = new AnalogIn(GP2_FRONTL);
	gp2_right = new AnalogIn(GP2_R);
	gp2_left = new AnalogIn(GP2_L);
}

Motor::Motor(Serial *pc_out) //Ajouter les pins dans les paramètres de constructeur
{
	pc = pc_out;
	buff_in = "";
	modeTest = true;
	status = false;
	//instEncoder = new Encoders(pc);
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

void Motor::link_to_enc(Encoders* enc) {
	instEncoder = enc;
}

void Motor::link_to_detect(GP2 *gp2) {
	sensor = gp2;
}

void Motor::stop(){
	routineAsserv->detach();
	motorD->write(0);
	motorG->write(0);
	status = false;
	hasToStop = false;
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

void Motor::position(float distance, float angle) {
	rotate_only = (distance == 0) ? true : false;
	if (rotate_only == true)
		interfaceCom->sendText("Rotate Only\n");
	int reading = 0;
	obstacle = false;
	led->write(0);
	//calc_vitesse();
	float distance_inc = distance*RESOLUTION / (PERIMETER);// *0.001);
	float angle_inc = 2 * angle*RADIUS_ENC*RESOLUTION / (PERIMETER);
	consigne_a_change(angle_inc);
	consigne_p_change(distance_inc);
	tc->reset();
	last_change_d = tc->read_us();
	routineAsserv->detach();
	routineAsserv->attach(callback(this, &Motor::asserv_position), PERIODE_ASSERV);
	if (modeTest) {
		pc->printf("Mode Test de Positionnement\n");
	}
	status = true;
}

void Motor::position_xy(float _x, float _y) {
	xi = instEncoder->getX();
	yi = instEncoder->getY();
	consigne_a_change(0);
	consigne_p_change(0);
	consigne_x = _x;
	consigne_y = _y;
	tc->reset();
	//last change pour condition d'arrêt à faire
	routineAsserv->detach();
	routineAsserv->attach(callback(this, &Motor::asserv_xy), PERIODE_ASSERV);
	status = true;
}

int Motor::stopped_obstacle() {
	return obstacle;
}

void Motor::asserv_position() {
	static float coeffXP[TAILLE_TABLEAUX] = { 0.07259, -0.06858, -0.07254, 0.06863 };// { 0.03564, -0.03161, -0.03561, 0.03165 }Ok;
	static float coeffYP[TAILLE_TABLEAUX] = { -1.334, -0.1061, 0.4404, 0 };// { 0.6591, -0.9903, 0.5688, 0 }Ok;
	static float coeffXA[TAILLE_TABLEAUX] = { 0.0363, -0.03429, -0.03627, 0.03432 };
	static float coeffYA[TAILLE_TABLEAUX] = { -1.334, -0.1061, 0.4404, 0 };
	//static float coeff = 0.00001;//0.000005;

	calc_vitesse();

	float dist_from_start = (dist_d + dist_g) / 2.;

	float epsilon_angle = consigne_parabolique_ang() - (dist_d - dist_g);
	float epsilon_dist = consigne_parabolique_pos() - dist_from_start;

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
	cd = cd*18*2/INC_MAX+0.03;
	cg = cg*18*2/INC_MAX+0.03;

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
	if ((fabs(consigne_position - dist_from_start) < 100) && (fabs(consigne_angle - (dist_d - dist_g)) < 100)) {
		interfaceCom->sendText("Position atteinte\n");
		stop();
	}	
	if ((rotate_only == false) && (sensor_status() == true)) {
		obstacle = true;
		interfaceCom->sendText("Obstacle detecte\n");
		stop();
	}

	if (tirette->read() > 90) {
		interfaceCom->sendText("Delai de jeu dépassé\n");
		stop();
	}
}

bool Motor::sensor_status() {
	bool out = false;
	if (gp2_front_l->read() > SEUIL_GP2) {
		interfaceCom->sendText("AVANT GAUCHE OBSTRUE\n");
		out = true;
	}
	if (gp2_front_r->read() > SEUIL_GP2) {
		interfaceCom->sendText("AVANT DROIT OBSTRUE\n");
		out = true;
	}
	if (gp2_right->read() > SEUIL_GP2+0.05) {
		interfaceCom->sendText("DROIT OBSTRUE\n");
		out = true;
	}
	if (gp2_left->read() > SEUIL_GP2+0.05) {
		interfaceCom->sendText("GAUCHE OBSTRUE\n");
		out = true;
	}
	return out;
}

void Motor::asserv_xy() {
	static float coeffXP[TAILLE_TABLEAUX] = { 0.07259, -0.06858, -0.07254, 0.06863 };// { 0.03564, -0.03161, -0.03561, 0.03165 }Ok;
	static float coeffYP[TAILLE_TABLEAUX] = { -1.334, -0.1061, 0.4404, 0 };// { 0.6591, -0.9903, 0.5688, 0 }Ok;
	static float coeffXA[TAILLE_TABLEAUX] = { 0.0363, -0.03429, -0.03627, 0.03432 };
	static float coeffYA[TAILLE_TABLEAUX] = { -1.334, -0.1061, 0.4404, 0 };
	calc_vitesse();

	float xk;
	float yk;
	consigne_parabolique_xy(&xk, &yk);
	float Dx = xk - instEncoder->getX();
	float Dy = yk - instEncoder->getY();
	int A = (Dx > 0) ? -1 : 1;
	float epsilon_angle = (Dy > 0) ? (A*atan(fabs(Dx) / fabs(Dy)) - instEncoder->getTheta()) : (A*PI - A*atan(fabs(Dx) / fabs(Dy)) - instEncoder->getTheta());
	float epsilon_dist = sqrt(Dx*Dx + Dy*Dy);

	push_in_tab(epsilon_dist, errors_dist);
	float vitesse = coeffXP[0] * errors_dist[0] + coeffXP[1] * errors_dist[1] + coeffXP[2] * errors_dist[2] + coeffXP[3] * errors_dist[3]
		- (coeffYP[0] * consignes_v[0] + coeffYP[1] * consignes_v[1] + coeffYP[2] * consignes_v[2]);
	push_in_tab(vitesse, consignes_v);
	push_in_tab(epsilon_angle, errors_angle);
	float angle = coeffXA[0] * errors_angle[0] + coeffXA[1] * errors_angle[1] + coeffXA[2] * errors_angle[2] + coeffXA[3] * errors_angle[3]
		- (coeffYA[0] * consignes_a[0] + coeffYA[1] * consignes_a[1] + coeffYA[2] * consignes_a[2]);
	push_in_tab(angle, consignes_a);

	float cd = vitesse + angle;
	float cg = vitesse - angle;
	//Normalisation
	cd = cd * 18 * 2 / INC_MAX + 0.03;
	cg = cg * 18 * 2 / INC_MAX + 0.03;

	if (cd < 0) {
		sens_mD->write(0);
		cd = -1 * cd;
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

	/*if (tc->read_us() - last_change_d > TIMEOUT_ASSERVPOS) {
		interfaceCom->sendText("Fin du processus d'asserv en position\n");
		stop();
	}
	if (pin->read() > SEUIL_GP2) {
		interfaceCom->sendText("Obstacle sur le trajet\n");
		obstacle = true;
		led->write(1);
		stop();
	}*/
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

	if (pwmd < 0) {
		sens_mD->write(0);
		pwmd = -1 * pwmd;
	}
	else {
		sens_mD->write(1);
	}
	if (pwmg < 0) {
		sens_mG->write(0);
		pwmg = -1 * pwmg;
	}
	else {
		sens_mG->write(1);
	}

	pwmd *= (pwmd > 0) * 2 - 1;
	pwmg *= (pwmg > 0) * 2 - 1;

	motorD->write(pwmd);
	motorG->write(pwmg);
}

float Motor::consigne_parabolique_pos() {
	float t = tc->read_us()*0.000001;
	float acceleration = ACCELERATION_POS;
	float vmax = VITESSEMAX_POS;
	
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

float Motor::consigne_parabolique_ang() {
	float t = tc->read_us()*0.000001;
	float acceleration = ACCELERATION_ANG;
	float vmax = VITESSEMAX_ANG;

	if (consigne_angle < 0) {
		acceleration *= -1;
		vmax *= -1;
	}

	float omega = consigne_angle / vmax + vmax / acceleration;
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
			return consigne_angle;
		}
	}
	else {
		omega = 2 * sqrt(consigne_angle / acceleration);
		gamma = omega / 2;
		if (t < gamma) {
			return 0.5*acceleration*t*t;
		}
		else if (t < omega) {
			return 0.5*acceleration*gamma*gamma + (acceleration*gamma)*(t - gamma) - 0.5*acceleration*(t - gamma)*(t - gamma);
		}
		else {
			return consigne_angle;
		}
	}
	return 0;
}

void Motor::consigne_parabolique_xy(float *_xk, float *_yk) {
	float t = tc->read_us()*0.000001;
	float acceleration = ACCELERATION_POS;
	float vmax = VITESSEMAX_POS;
	
	//Calcul du xy théorique
	float dist_ab = sqrt((consigne_x - xi)*(consigne_x - xi) + (consigne_y - yi)*(consigne_y - yi));
	float facteur = 0.5;
		//Calcul du facteur
	float omega = dist_ab / vmax + vmax / acceleration;
	float alpha = vmax / acceleration;
	float beta = omega - vmax / acceleration;
	float gamma;

	if (alpha < beta) {
		if (t < alpha) {
			facteur = (0.5*acceleration*t*t)/dist_ab;
		}
		else if (t < beta) {
			facteur = (0.5*acceleration*alpha*alpha + vmax*(t - alpha))/dist_ab;
		}
		else if (t < omega) {
			facteur = (0.5*acceleration*alpha*alpha + vmax*(beta - alpha) + vmax*(t - beta) - 0.5*acceleration*(t - beta)*(t - beta))/dist_ab;
		}
		else {
			facteur = 1;
		}
	}
	else {
		omega = 2 * sqrt(consigne_position / acceleration);
		gamma = omega / 2;
		if (t < gamma) {
			facteur = (0.5*acceleration*t*t)/dist_ab;
		}
		else if (t < omega) {
			facteur = (0.5*acceleration*gamma*gamma + (acceleration*gamma)*(t - gamma) - 0.5*acceleration*(t - gamma)*(t - gamma))/dist_ab;
		}
		else {
			facteur = 1;
		}
	}

	*_xk = (consigne_x - xi)*facteur/dist_ab;
	*_yk = (consigne_y - yi)*facteur/dist_ab;	
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
	float omega = consigne_position / VITESSEMAX_POS + VITESSEMAX_POS / ACCELERATION_POS;
	float alpha = VITESSEMAX_POS / ACCELERATION_POS;
	float beta = omega - VITESSEMAX_POS / ACCELERATION_POS;
	interfaceCom->set_pos(instEncoder->getX(), instEncoder->getY(), instEncoder->getTheta());
	interfaceCom->set_tinfo(t, omega);
	interfaceCom->set_dist_d(instEncoder->getDd());//consigne_parabolique_pos());
	interfaceCom->set_dist_g(instEncoder->getDg());//consigne_parabolique_ang());
	interfaceCom->set_erreur_pos(consigne_position - ((dist_d + dist_g) / 2.));
	interfaceCom->set_erreur_ang(consigne_angle - dist_d + dist_g);
	interfaceCom->set_pwmd(pwmd);
	interfaceCom->set_pwmg(pwmg);
	interfaceCom->set_status_motor(status);
	interfaceCom->set_bat18(18);
	interfaceCom->set_bat9(9);
	interfaceCom->set_current_d(1);
	interfaceCom->set_current_g(-1);
}

bool Motor::get_state() {
	return status;
}

Motor::~Motor()
{
}
