#pragma once

#include <mbed.h>
#include "PINOUT_NUCLEO.hpp"
#include "Ticker.h"

class GP2{
  public:
    GP2(double);
    void detect();
	int isTooClose();
	bool getState();

    //getter and setter
    void setSeuil(double);
    double getSeuil();

  private:
    double voltFromDistance(double);

    AnalogIn* pin;
    double seuil_distance;
    double seuil_volt;
	float valeur;
	int stop;
};
