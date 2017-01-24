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
		void asserv_vitesse(float vitesse); //Speed in mm/s
		void asserv_position(float distance); //Distance to travel in mm
		void asserv_angle(float angle); //Angle to rotate in degrees
		void routine();
		void calc_sens(float vd, float vg);
		void calc_vitesse();
		void test_encodeurs();
		~Motor();
	private:
		void push_in_tab(float x, float tableau[]);
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
		float consigne_position; //In Imp
		float vitesse_d; //In Imp.s^-1
		float vitesse_g; //In Imp.s^-1
		int imp_d; //Right Encoder Impulsions Count
		int imp_g; //Left Encoder Impulsions Count
		float pwmd; //Right Motor PWM
		float pwmg; //Left Motor PWM
		float vs_d[8];
		float vs_g[8];
		float errors_d[8];
		float errors_g[8];
};