#include "mbed.h"

#include "Navigator.hpp"
#include "Motor.hpp"
#include "UIcom.hpp"
#include "CONSTANTS.hpp"
#include "PINOUT_NUCLEO.hpp"

int main() {
	//Serial *pc = new Serial(USBTX, USBRX, 9600);
	//Motor instMotor(pc);
	std::vector<vector<float> > route_test1;
	std::vector<float> pt10(2, 0);//Position initiale;
	pt10[0] = 0;
	pt10[1] = 0;
	std::vector<float> pt11(2, 0);
	pt11[0] = 1000;
	pt11[1] = 0;
	route_test1.push_back(pt10);

	std::vector<vector<float> > route_test2;
	std::vector<float> pt20(2, 0);//Position initiale;
	pt20[0] = 0;
	pt20[1] = 0;
	std::vector<float> pt21(2, 0);
	pt21[0] = 1000;
	pt21[1] = 0;
	std::vector<float> pt22(2, 0);
	pt22[0] = 1000;
	pt22[1] = 1000;
	route_test2.push_back(pt21);
	route_test2.push_back(pt22);
	route_test2.push_back(pt21);
	route_test2.push_back(pt22);
	route_test2.push_back(pt21);
	route_test2.push_back(pt20);

	std::vector<vector<float> > route;
	std::vector<float> p0(2, 0);//Position initiale;
	p0[0] = 880;
	p0[1] = 180;
	std::vector<float> p1(2, 0);
	p1[0] = 880;
	p1[1] = 1380;
	//std::vector<float> p2(2, 0);
	route.push_back(p1);
	//route.push_back(p2);

	Navigator nav(pt10[0], pt10[1]);
	nav.navigate(&route_test1);

	//Motor instMotor;
	//UIcom ui(&instMotor);
	//instMotor.link_to_ui(&ui);

	//instMotor.position(20000, 0);
	/*while (1) {
		wait(2);
		test.navigate();
	}*/

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