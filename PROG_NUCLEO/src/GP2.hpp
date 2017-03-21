#pragma once

#include "mbed.h"
#include "PINOUT_NUCLEO.hpp"

class GP2{
  public:
    GP2(double);
    bool detect();

    //getter and setter
    void setSeuil(double);
    double getSeuil();

  private:
    double voltFromDistance(double);

    AnalogIn* pin;
    double seuil_distance;
    double seuil_volt;
};
