#pragma once

#include "mbed.h"
#include "CONSTANTS.hpp"
#include "PINOUT_NUCLEO.hpp"

class Proto
{
	public:
		Proto();
		~Proto();
		void recup();
		void pick_and_keep();
		void drop_pince();
		void down_arm();
		void up_arm();
		void testProt();
		void fermeture_pince();
		void ouverture_pince();
		void leve_bras();

	private:		
		void descente_bras();		
		void free_cylindre();

		AnalogIn *gp2fr;
		AnalogIn *gp2fl;
		bool obstructed();

		PwmOut *pince;
		PwmOut *bras;
		PwmOut *reserve;
};

