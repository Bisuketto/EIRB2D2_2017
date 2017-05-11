#pragma once

#include "mbed.h"
#include "PINOUT_NUCLEO.hpp"
#include <vector>

class PathFinderCom
{
	public:
		PathFinderCom();
		~PathFinderCom();
		std::vector<float> getNext(float x_dest, float y_dest, float xi, float yi);
	private:
		Serial *raspCom;
};

