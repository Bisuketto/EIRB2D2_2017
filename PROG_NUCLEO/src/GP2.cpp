#include "GP2.hpp"

GP2::GP2(double seuil_d){
    seuil_distance = seuil_d;
    seuil_volt = voltFromDistance(seuil_distance);

    pin = new AnalogIn(PIN_GP2);
}

bool GP2::detect(){  //Comparaison par tensions
    return seuil_volt > pin->read();
}


double GP2::voltFromDistance(double dist){
    return 25.1 * pow(dist,-0.965);
}

//getter and setter
void GP2::setSeuil(double seuil_d){
    seuil_distance = seuil_d;
    seuil_volt = voltFromDistance(seuil_distance);
}

double GP2::getSeuil(){
    return seuil_distance;
}
