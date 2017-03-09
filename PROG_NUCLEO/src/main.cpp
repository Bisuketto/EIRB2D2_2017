#include "mbed.h"

#include "Motor.hpp"
#include "UIcom.hpp"
#include "CONSTANTS.hpp"
#include "PINOUT_NUCLEO.hpp"

int main() {
	//Serial *pc = new Serial(USBTX, USBRX, 9600);
	//Motor instMotor(pc);


	Motor instMotor;
	UIcom ui(&instMotor);
	instMotor.link_to_ui(&ui);
	//instMotor.position(20000, 0);
	while (1);

	/*char buff[64] = "";
	float consigne = 0;
	while (1) {
		pc->gets(buff, 64);
		pc->printf("Topkek ");
		consigne = atof(buff);
		for(int i = 0; i < 10; i++){
			pc->printf("%d ", buff[i]);
		}
		pc->printf("\n");
		pc->printf(" %f\n", consigne);
		instMotor.position(consigne, 0);
		for (int j = 0; j < 64; j++)
			buff[j] = 0;
	}
	*/
	return 0;
}