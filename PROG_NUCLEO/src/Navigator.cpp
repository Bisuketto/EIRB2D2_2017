#include "Navigator.hpp"

Navigator::Navigator()
{
	encoders = new Encoders(0, 0, 0);
	motorisation = new Motor();
	interfaceCom = new UIcom(motorisation);
	instGP2 = new GP2(SEUIL_GP2);
	interCote = new DigitalIn(PIN_SWITCHSIDE);
	instPF = new PathFinderCom();
	cote = (interCote->read() == 0) ? YELLOW : BLUE;
	DigitalOut cote_bleu(PIN_LED_BLEUE);
	DigitalOut cote_jaune(PIN_LED_JAUNE);
	cote_bleu.write(1);
	cote_jaune.write(1);
	if (cote == YELLOW) {
		cote_jaune.write(0);
		interfaceCom->sendText("Init cote Jaune\n");
	}
	else {
		cote_bleu.write(0);
		interfaceCom->sendText("Init cote Bleu\n");
	}

	motorisation->link_to_ui(interfaceCom);
	motorisation->link_to_enc(encoders);
	motorisation->link_to_detect(instGP2);
}

Navigator::Navigator(float x0, float y0, float theta0) {
	motorisation = new Motor();
	interfaceCom = new UIcom(motorisation);
	interCote = new DigitalIn(PIN_SWITCHSIDE);
	instPF = new PathFinderCom();
	cote = (interCote->read() == 0) ? YELLOW : BLUE;
	DigitalOut cote_bleu(PIN_LED_BLEUE);
	DigitalOut cote_jaune(PIN_LED_JAUNE);
	cote_bleu.write(1);
	cote_jaune.write(1);
	if (cote == YELLOW) {
		cote_jaune.write(0);
		interfaceCom->sendText("Init cote Jaune\n");
	}
	else {
		cote_bleu.write(0);
		interfaceCom->sendText("Init cote Bleu\n");
	}

	float x0_reel = (cote == YELLOW) ? x0 : 3000 - x0;
	float y0_reel = (cote == YELLOW) ? y0 : y0;
	float th0_reel = (cote == YELLOW) ? theta0 : -theta0;
	encoders = new Encoders(x0_reel, y0_reel, th0_reel);

	motorisation->link_to_ui(interfaceCom);
	motorisation->link_to_enc(encoders);
	//instGP2 = new GP2(SEUIL_GP2);
}

Navigator::~Navigator()
{
}

void Navigator::chemin1() {
	std::vector<vector<float> > portion;
	std::vector<float> p11(2, 0);
	p11[0] = 880;
	p11[1] = 175;
	std::vector<float> p12(2, 0);
	p12[0] = 1260;
	p12[1] = 600;
	portion.push_back(p11);
	portion.push_back(p12);
	navigate(portion);
}

void Navigator::retour1() {
	std::vector<vector<float> > portion;
	std::vector<float> p11(2, 0);
	p11[0] = 1350;
	p11[1] = 250;
	portion.push_back(p11);
	navigate(portion);
}

void Navigator::chemin2() {
	std::vector<vector<float> > portion;
	std::vector<float> p11(2, 0);
	p11[0] = 1250;
	p11[1] = 600;
	std::vector<float> p12(2, 0);
	p12[0] = 770;
	p12[1] = 1075;
	portion.push_back(p11);
	portion.push_back(p12);
	navigate(portion);
}

void Navigator::retour2() {
	std::vector<vector<float> > portion;
	std::vector<float> p11(2, 0);
	p11[0] = 1250;
	p11[1] = 600;
	std::vector<float> p12(2, 0);
	p12[0] = 1300;
	p12[1] = 250;
	portion.push_back(p11);
	portion.push_back(p12);
	navigate(portion);
}

void Navigator::debug_motor(float x, float y) {
	interfaceCom->sendText("Début du test xy\n");
	motorisation->position_xy(x, y);
	while (motorisation->get_state() == true);
}

void Navigator::navigate(std::vector<vector<float> > route) {
	int state;
	char sout[64] = "";
	for (int i = 0; i < route.size(); i++) {
		sprintf(sout, "Point %d\n", i);
		interfaceCom->sendText(sout);
		sprintf(sout, "xp %f yp %f\n", (route)[i][0], (route)[i][1]);
		interfaceCom->sendText(sout);
		do{
			state = go_to((route)[i][0], (route)[i][1]);
			if (state == -1)
				wait(1.5);
		} while (state == -1);
		sprintf(sout, "Ok\n");
		interfaceCom->sendText(sout);
	}

	sprintf(sout, "Go to angle: %f\n", encoders->getTheta()*180/PI);
	interfaceCom->sendText(sout);
	//motorisation->position(0, -(encoders->getTheta()));
	//while (motorisation->get_state() == true)
		
	sprintf(sout, "Navigation terminée\n");
	interfaceCom->sendText(sout);
}

void Navigator::nav_pf(std::vector<float> dest) {
	std::vector<float> pt;
	float x = encoders->getX();
	float y = encoders->getY();
	while(fabs(dest[0] - x) > MARGE_NAV && fabs(dest[1] -y) > MARGE_NAV){
		pt = instPF->getNext(dest[0], dest[1], x, y);
		go_to(pt[0], pt[1]);
	}
}

int Navigator::orientation(float theta) {
	float th_reel = (cote == YELLOW) ? theta : -theta;
	float theta_act = encoders->getTheta();
	float angle = th_reel - theta_act;
	motorisation->position(0, angle);
	while (motorisation->get_state() == true);
	if (motorisation->stopped_obstacle()) {
		return -1;
	}
	return 0;
}

int Navigator::go_to(float x, float y) {
	float x_reel = (cote == YELLOW) ? x : 3000 - x;
	float y_reel = (cote == YELLOW) ? y : y;

	float Dx = x_reel - encoders->getX();
	float Dy = y_reel - encoders->getY();
	float theta_act = encoders->getTheta();

	char sout[64] = "";
	sprintf(sout, "xi: %f yi: %f thi: %f\n", encoders->getX(), encoders->getY(), encoders->getTheta());
	interfaceCom->sendText(sout);
	
	int A = (Dx > 0) ? -1 : 1;
	float angle = (Dy > 0) ? (A*atan(fabs(Dx) / fabs(Dy)) - theta_act) : (A*PI - A*atan(fabs(Dx) / fabs(Dy)) - theta_act);
	float pos = sqrt(Dx*Dx + Dy*Dy);

	sprintf(sout, "Go to angle: %f\n", angle*180/PI);
	interfaceCom->sendText(sout);
	motorisation->position(0, angle);
	while (motorisation->get_state() == true);
	if (motorisation->stopped_obstacle()) {
		return -1;
	}

	sprintf(sout, "Go to pos: %f\n", pos);
	interfaceCom->sendText(sout);
	motorisation->position(pos, 0);
	while (motorisation->get_state() == true);
	if (motorisation->stopped_obstacle() == true) {
		return -1;
	}
	
	return 0;
}