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
		float epsilon_v_dant; //Right Speed Error n-1
		float epsilon_v_gant; //Left Speed Error n-1
		float sum_epsilon_v_d; //Right Speed Error Sum
		float sum_epsilon_v_g; //Left Speed Error Sum
};