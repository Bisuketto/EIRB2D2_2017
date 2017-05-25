#pragma once

#include <mbed.h>
#include <vector>
#include <math.h>
#include "CONSTANTS.hpp"
#include "UIcom.hpp"
#include "Encoders.hpp"
#include "Motor.hpp"
#include "GP2.hpp"
#include "PathFinderCom.hpp"

enum side{YELLOW = 0, BLUE = 1};

class Navigator
{
	public:
		Navigator();
		Navigator(float x0, float y0, float theta0 = 0);
		~Navigator();
		void navigate(std::vector<vector<float> > route);
		void chemin1();
		void retour1();
		void chemin2();
		void retour2();
		void nav_pf(std::vector<float> dest);
		int orientation(float theta);

		void debug_motor(float x, float y);

	private:
		int go_to(float x, float y);
		enum side cote;
		UIcom *interfaceCom;
		Encoders *encoders;
		Motor *motorisation;
		GP2 *instGP2;
		DigitalIn *interCote;
		PathFinderCom *instPF;
};

