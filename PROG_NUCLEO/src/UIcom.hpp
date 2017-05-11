#pragma once

#include "mbed.h"
#include "CONSTANTS.hpp"
#include "Motor.hpp"
#include <string>

class Motor;

class UIcom
{
	public:
		UIcom(Motor *instMotor);
		void sendText(char *str);
		void set_erreur_pos(float value);
		void set_erreur_ang(float value);
		void set_pwmd(float value);
		void set_pwmg(float value);
		void set_dist_d(float value);
		void set_dist_g(float value);
		void set_current_d(float value);
		void set_current_g(float value);
		void set_bat9(float value);
		void set_bat18(float value);
		void set_status_motor(bool value);
		void set_pos(float xi, float yi, float thetai);
		void set_tinfo(float ti, float omegai);
		~UIcom();
	private:
		bool status; //True for busy
		bool status_motor;
		void get_instr();
		void send_Infos();
		float erreur_pos;
		float erreur_ang;
		float pwmg;
		float pwmd;
		float dist_g;
		float dist_d;
		float current_g;
		float current_d;
		float bat9;
		float bat18;
		float omega;
		float t;
		float x;
		float y;
		float theta;
		Motor *ptomotor;
		Serial *serialCom;
		Ticker *scheduleOut;
};

