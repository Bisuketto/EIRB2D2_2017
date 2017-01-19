#pragma once

#include "CONSTANTS.hpp"
#include "PINOUT_NUCLEO.hpp"
#include "mbed.h"

class Encoders
{
	public:
		Encoders();
		Encoders(Serial *pcOut);
		int getImpEncD();
		int getImpEncG();
		void routine_Encoders();
		~Encoders();
	private:
		bool modeTest;
		Serial *pc;
		int impEncD;
		int impEncG;
		void TIM3_EncoderInit();
		void TIM4_EncoderInit();
		Ticker *scheduler_Encoders;
};