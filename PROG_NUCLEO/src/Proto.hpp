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
	private:
		void fermeture_pince();
		void ouverture_pince();
		void descente_bras();
		void leve_bras();

		PwmOut *pince;
		PwmOut *bras;
};

