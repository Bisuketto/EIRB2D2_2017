#include "Navigator.hpp"

Navigator::Navigator()
{
	encoders = new Encoders(0, 0, 0);
	motorisation = new Motor();
	interfaceCom = new UIcom(motorisation);
	motorisation->link_to_ui(interfaceCom);
	motorisation->link_to_enc(encoders);
	instGP2 = new GP2(SEUIL_GP2);
}

Navigator::Navigator(float x0, float y0, float theta0) {
	encoders = new Encoders(x0, y0, theta0);
	motorisation = new Motor();
	interfaceCom = new UIcom(motorisation);
	motorisation->link_to_ui(interfaceCom);
	motorisation->link_to_enc(encoders);
	instGP2 = new GP2(SEUIL_GP2);
}

Navigator::~Navigator()
{
}

void Navigator::navigate(std::vector<vector<float> > *route) {
	char sout[64] = "";
	for (int i = 0; i < route->size(); i++) {
		sprintf(sout, "Point %d\n", i);
		interfaceCom->sendText(sout);
		go_to((*route)[i][0], (*route)[i][1]);
		sprintf(sout, "Ok\n");
		interfaceCom->sendText(sout);
	}

	sprintf(sout, "Go to angle: %f\n", encoders->getTheta()*180/PI);
	interfaceCom->sendText(sout);
	motorisation->position(0, -(encoders->getTheta()));
	while (motorisation->get_state() == true);
	sprintf(sout, "Navigation terminée\n");
	interfaceCom->sendText(sout);
}

int Navigator::go_to(float x, float y) {
	float Dx = x - encoders->getX();
	float Dy = y - encoders->getY();
	float theta_act = encoders->getTheta();

	char sout[64] = "";
	sprintf(sout, "xi: %f yi: %f thi: %f\n", encoders->getX(), encoders->getY(), encoders->getTheta());
	interfaceCom->sendText(sout);
	
	int A = (Dx > 0) ? -1 : 1;
	float angle = (Dy > 0) ? (A*atan(fabs(Dx) / fabs(Dy)) - theta_act) : (PI - A*atan(fabs(Dx) / fabs(Dy)) - theta_act);
	float pos = sqrt(Dx*Dx + Dy*Dy);

	sprintf(sout, "Go to angle: %f\n", angle*180/PI);
	interfaceCom->sendText(sout);
	motorisation->position(0, angle);
	while (motorisation->get_state() == true);

	sprintf(sout, "Go to pos: %f\n", pos);
	interfaceCom->sendText(sout);
	motorisation->position(pos, 0);
	while (motorisation->get_state() == true);
	
	return 0;
}