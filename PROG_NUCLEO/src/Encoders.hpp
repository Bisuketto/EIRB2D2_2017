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
		int getImpEncD();
		int getImpEncG();
		void routine_Encoders();
		void odometrie();
		int getX();
		int getY();
		~Encoders();
	private:
		bool modeTest;
		Serial *pc;
		int impEncD;
		int impEncG;
		void TIM3_EncoderInit();
		void TIM4_EncoderInit();
		Ticker *scheduler_Encoders;
		//Implementation de l'odometrie, les unités sont en ticks
		float dtot_d;
		float dtot_g;
		float vg;
		float vd;
		float x;
		float y;
		float theta; //Orientation
		float distance;
		float v;
		float Rlocal; //Rayon de courbure local


};