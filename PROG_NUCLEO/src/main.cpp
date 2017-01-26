#include "mbed.h"

#include "Motor.hpp"
#include "CONSTANTS.hpp"
#include "PINOUT_NUCLEO.hpp"

int main() {
	Serial *pc = new Serial(USBTX, USBRX, 9600);
	pc->printf("Attente...\n");
	DigitalIn button(USER_BUTTON);
	while (button.read() == 0);
	Motor instMotor(pc);
	instMotor.calc_sens(1, 1);
	instMotor.asserv_vitesse(50);
	return 0;
}