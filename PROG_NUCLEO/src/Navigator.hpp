#pragma once

#include <mbed.h>
#include <vector>
#include <math.h>
#include "CONSTANTS.hpp"
#include "UIcom.hpp"
#include "Encoders.hpp"
#include "Motor.hpp"
#include "GP2.hpp"

class Navigator
{
	public:
		Navigator();
		Navigator(float x0, float y0, float theta0 = 0);
		~Navigator();
		void navigate(std::vector<vector<float> > *route);

	private:
		int go_to(float x, float y);
		UIcom *interfaceCom;
		Encoders *encoders;
		Motor *motorisation;
		GP2 *instGP2;
};

