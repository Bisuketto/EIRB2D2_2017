#pragma once

#include "CONSTANTS.hpp"
#include "PINOUT_NUCLEO.hpp"
#include <math.h>
#include "mbed.h"

class Encoders
{
	public:
		Encoders();
		Encoders(Serial *pcOut);
		Encoders(float x0, float y0, float theta_i);
		int getImpEncD();
		int getImpEncG();
		float getX();
		float getY();
		float getTheta();
		float getDg();
		float getDd();
		~Encoders();
	private:
		void routine_Encoders();
		void odometrie();
		bool modeTest;
		Serial *pc;
		int impEncD;
		int impEncG;
		void TIM3_EncoderInit();
		void TIM4_EncoderInit();
		Ticker *scheduler_Encoders;
		//Implementation de l'odometrie, les unités sont en ticks
		float theta0;
		int dtot_d;
		int dtot_g;
		float dlg;
		float dld;
		float x;
		float y;
		float theta; //Orientation en rad
		float dl;
};