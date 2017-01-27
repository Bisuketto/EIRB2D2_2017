#pragma once

#include "mbed.h"
#include "CONSTANTS.hpp"

class UIcom
{
	public:
		UIcom();
		void set_erreur_d(float value);
		void set_erreur_g(float value);
		void set_pwmd(float value);
		void set_pwmg(float value);
		void set_dist_d(float value);
		void set_dist_g(float value);
		void set_current_d(float value);
		void set_current_g(float value);
		void set_bat9(float value);
		void set_bat18(float value);
		~UIcom();
	private:
		void send_Infos();
		float erreur_d;
		float erreur_g;
		float pwmg;
		float pwmd;
		float dist_g;
		float dist_d;
		float current_g;
		float current_d;
		float bat9;
		float bat18;
		Serial *serialCom;
		Ticker *schedule;
};

