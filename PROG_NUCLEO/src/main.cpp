#include "mbed.h"

#include "Navigator.hpp"
#include "Proto.hpp"
#include "CONSTANTS.hpp"
#include "PINOUT_NUCLEO.hpp"
#include "GP2.hpp"

int main() {
	/*
	DigitalOut v(LED1);
	GP2 test(30);
	Serial loul(USBTX, USBRX, 115200);
	loul.printf("Toopjeej\n");
	while (1) {
		int lul = test.isTooClose();
		v.write(lul);
		//loul.printf("%f\n", test.getget());
		//wait(1);
	}*/
	DigitalIn a(PG_9);
	DigitalIn btn(USER_BUTTON);
	//while (a.read() == 1); //Tirette
	DigitalOut blue(LED2);
	DigitalOut red(LED3);
	blue.write(1);
	red.write(1);

	Proto *instProto = new Proto();
	while (btn.read() == 0);
	
	Navigator nav1(175, 175, -PI/2.);

	//nav1.debug_motor(800, 175);

	//while (btn.read() == 0);

	std::vector<vector<float> > portion1;
	std::vector<float> p0_test(2, 0);
	p0_test[0] = 175;
	p0_test[1] = 175;
	std::vector<float> p1_test(2, 0);
	p1_test[0] = 880;
	p1_test[1] = 175;
	std::vector<float> p2_test(2, 0);
	p2_test[0] = 1260;
	p2_test[1] = 600;
	portion1.push_back(p1_test);
	portion1.push_back(p2_test);
	nav1.navigate(portion1);
	nav1.orientation(PI / 2);
	instProto->pick_and_keep();
	std::vector<vector<float> > retour1;
	std::vector<float> r11(2, 0);
	r11[0] = 1350;
	r11[1] = 250;
	
	retour1.push_back(r11);
	nav1.navigate(retour1);
	nav1.orientation(PI / 2);
	instProto->drop_pince();

	std::vector<vector<float> > portion2;
	std::vector<float> p21(2, 0);	
	p21[0] = 1250;
	p21[1] = 600;
	std::vector<float> p22(2, 0);
	p22[0] = 770;
	p22[1] = 1075;
	portion2.push_back(p21);
	portion2.push_back(p22);
	nav1.navigate(portion2);
	nav1.orientation(PI / 2);
	instProto->pick_and_keep();

	std::vector<vector<float> > retour2;
	std::vector<float> r21(2, 0);
	r21[0] = 1250;
	r21[1] = 600;
	std::vector<float> r22(2, 0);
	r22[0] = 1300;
	r22[1] = 250;
	retour2.push_back(r21);
	retour2.push_back(r22);
	nav1.navigate(retour2);
	nav1.orientation(PI / 2 + PI/6);
	instProto->drop_pince();

	std::vector<vector<float> > portion3;
	std::vector<float> p31(2, 0);
	p31[0] = 1260;
	p31[1] = 600;
	std::vector<float> p32(2, 0);
	p32[0] = 700;
	p32[1] = 1075;
	std::vector<float> p33(2, 0);
	p33[0] = 415;
	p33[1] = 820;
	portion3.push_back(p31);
	portion3.push_back(p32);
	portion3.push_back(p33);
	nav1.navigate(portion3);
	nav1.orientation(3 * PI / 4);
	instProto->pick_and_keep();

	std::vector<vector<float> > retour3;
	std::vector<float> r31(2, 0);
	r31[0] = 350;
	r31[1] = 700;
	retour3.push_back(r31);
	nav1.navigate(retour3);
	nav1.orientation(PI);
	instProto->drop_pince();


	/*
	std::vector<vector<float> > portion4;
	std::vector<float> p41(2, 0);
	p41[0] = 650;
	p41[1] = 1600;
	portion4.push_back(p41);
	nav1.navigate(portion4);
	nav1.orientation(- PI / 6);
	instProto->recup();*/

	/*
	std::vector<vector<float> > r2;
	std::vector<float> p3_test(2, 0);
	p3_test[0] = 700;
	p3_test[1] = 900;
	std::vector<float> p4_test(2, 0);
	p4_test[0] = 434;
	p4_test[1] = 735;
	r2.push_back(p3_test);
	r2.push_back(p4_test);
	nav1.navigate(r2);
	//Prendre Cylindre Angle 120°
	std::vector<vector<float> > r3;
	std::vector<float> p5_test(2, 0);
	p5_test[0] = 270;
	p5_test[1] = 800;
	r3.push_back(p5_test);
	nav1.navigate(r3);
	//Lacher de Cylindre Angle 180°
	std::vector<vector<float> > r4;
	std::vector<float> p6_test(2, 0);
	p6_test[0] = 270;
	p6_test[1] = 1000;
	r4.push_back(p6_test);
	nav1.navigate(r4);
	//Lacher de Cylindre Angle 180°
	std::vector<vector<float> > r5;
	std::vector<float> p7_test(2, 0);
	p7_test[0] = 175;
	p7_test[1] = 175;
	r5.push_back(p7_test);
	nav1.navigate(r5);
	//Prendre Cylindre Angle ?
	std::vector<vector<float> > r6;
	std::vector<float> p8_test(2, 0);
	p8_test[0] = 270;
	p8_test[1] = 600;
	r6.push_back(p8_test);
	nav1.navigate(r6);
	//Lacher de Cylindre Angle 180°
	*/

	
	//Serial *pc = new Serial(USBTX, USBRX, 9600);
	//Motor instMotor(pc);

	
	/*Navigator navTest(150, 150);
	std::vector<vector<float> > route_test1;
	std::vector<vector<float> > alleretretourdunhobbit_parbilbosacque;
	std::vector<float> pt10(2, 0);//Position initiale;
	pt10[0] = 150;
	pt10[1] = 150;
	std::vector<float> pt11(2, 0);
	pt11[0] = 150;
	pt11[1] = 550;//1150;
	std::vector<float> pt12(2, 0);
	pt12[0] = 150;
	pt12[1] = 150;
	std::vector<float> pt13(2, 0);
	pt13[0] = 400;
	pt13[1] = 150;
	std::vector<float> pt14(2, 0);
	pt14[0] = 400;
	pt14[1] = 1150;
	route_test1.push_back(pt11);
	route_test1.push_back(pt12);
	route_test1.push_back(pt13);
	route_test1.push_back(pt14);
	route_test1.push_back(pt10);
	alleretretourdunhobbit_parbilbosacque.push_back(pt11);
	alleretretourdunhobbit_parbilbosacque.push_back(pt10);
	navTest.navigate(alleretretourdunhobbit_parbilbosacque);
	navTest.navigate(alleretretourdunhobbit_parbilbosacque);
	navTest.navigate(alleretretourdunhobbit_parbilbosacque);
	navTest.navigate(route_test1);
	navTest.orientation(PI);
	while (btn.read() == 0);
	navTest.orientation(0);
	instProto->recup();*/
	


	std::vector<vector<float> > route_test2;
	std::vector<float> pt20(2, 0);//Position initiale;
	pt20[0] = 172;
	pt20[1] = 172;
	std::vector<float> pt21(2, 0);
	pt21[0] = 870;
	pt21[1] = 172;
	std::vector<float> pt22(2, 0);
	pt22[0] = 870+300;
	pt22[1] = 500+172;
	route_test2.push_back(pt21);
	//route_test2.push_back(pt22);

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

	//Navigator nav(pt10[0], pt10[1]);
	//nav.navigate(route_test1);

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