#include "GP2.hpp"

GP2::GP2(double seuil_d){
	stop = false;
    seuil_distance = seuil_d;
    seuil_volt = voltFromDistance(seuil_distance);
	pin = new AnalogIn(PIN_GP2);
	valeur = 0;
}

void GP2::detect(){  //Comparaison par tensions
	float posV = pin->read();
	stop = posV > 0.90;//seuil_volt > posV;
	return;
}

int GP2::isTooClose() {
	detect();
	return stop;
}

double GP2::voltFromDistance(double dist){
	return 25.1 / dist; //* pow(dist,-0.965);
}

//getter and setter
void GP2::setSeuil(double seuil_d){
    seuil_distance = seuil_d;
    seuil_volt = voltFromDistance(seuil_distance);
}

double GP2::getSeuil(){
    return seuil_distance;
}
