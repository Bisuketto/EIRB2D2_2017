#pragma once

#include "mbed.h"
#include "CONSTANTS.hpp"
#include "PINOUT_NUCLEO.hpp"
#include "Encoders.hpp"

class Motor
{
	public:
		Motor();
		Motor(Serial *pc_out);
		void asserv_vitesse(float vitesse);
		void routine();
		void calc_sens(float vd, float vg);
		void calc_vitesse();
		void test_encodeurs();
		~Motor();
	private:
		Encoders *instEncoder;
		Ticker *routineAsserv;
		Timer *affichage_debug;
		Serial *pc;
		bool modeTest;
		PwmOut *motorD;
		PwmOut *motorG;
		DigitalIn * bouton;
		DigitalOut *sens_mD;
		DigitalOut *sens_mG;
		int sensG;
		int sensD;
		float consigne_vitesse; //In Imp.s^-1
		float vitesse_d; //In Imp.s^-1
		float vitesse_g; //In Imp.s^-1
		int imp_d;
		int imp_g;
		float dist_d;
		float dist_g;
		float pwmd_eff;
		float pwmg_eff;
};