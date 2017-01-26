#pragma once

#include "mbed.h"
#include "CONSTANTS.hpp"

class UIcom
{
	public:
		UIcom();
		void set_erreur_d();
		void set_erreur_g();
		void set_pwmd();
		void set_pwmg();
		void set_dist_d();
		void set_dist_g();
		void set_current_d();
		void set_current_g();
		void set_bat9();
		void set_bat18();
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

