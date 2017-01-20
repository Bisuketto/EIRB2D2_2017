#include "mbed.h"

#include "Motor.hpp"
#include "CONSTANTS.hpp"
#include "PINOUT_NUCLEO.hpp"

int main() {
	Serial *pc = new Serial(USBTX, USBRX, 9600);
	pc->printf("Attente...\n");
	DigitalIn button(USER_BUTTON);
	while (button.read() == 0);
	Motor instMotor;
	instMotor.asserv_vitesse(200);
	return 0;
}