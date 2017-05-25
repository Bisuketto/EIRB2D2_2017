#include "Proto.hpp"

Proto::Proto()
{
	bras = new PwmOut(PIN_BRAS);
	pince = new PwmOut(PIN_PINCE);
	reserve = new PwmOut(PIN_RESERVE);

	gp2fr = new AnalogIn(GP2_FRONTR);
	gp2fl = new AnalogIn(GP2_FRONTL);

	bras->period_ms(20);
	reserve->period_ms(20);
	pince->period_ms(30);

	reserve->pulsewidth_us(600);
	pince->pulsewidth_us(2300);
	bras->pulsewidth_us(900);
}


Proto::~Proto()
{
}

void Proto::fermeture_pince() {
	pince->pulsewidth_us(3000);
}

void Proto::ouverture_pince() {
	pince->pulsewidth_us(1400);
}

void Proto::descente_bras() {
	while (obstructed())
		wait(0.2);
	bras->pulsewidth_us(1800);
	wait(1);
	bras->write(0);
}

void Proto::leve_bras() {
	bras->pulsewidth_us(900);
}

void Proto::free_cylindre() {
	reserve->pulsewidth_us(400);
	//wait(1);
	//reserve->pulsewidth_us(600);
	//wait(1);
}

void Proto::testProt() {
	fermeture_pince();
	wait(2.5);
	ouverture_pince();
	wait(2.5);
	fermeture_pince();
	wait(2.5);
	//descente_bras();
	//wait(1.);
	//leve_bras();
	//wait(1.);
	//free_cylindre();
	//wait(1.);
}

void Proto::recup() {
	ouverture_pince();
	wait(1);
	descente_bras();
	wait(1);
	fermeture_pince();
	wait(2);
	leve_bras();
	wait(1);
	ouverture_pince();
	wait(1);
}

void Proto::pick_and_keep() {
	ouverture_pince();
	wait(1);
	descente_bras();
	wait(1);
	fermeture_pince();
	wait(2);
	leve_bras();
	wait(1);
}

void Proto::drop_pince() {
	descente_bras();
	wait(1);
	ouverture_pince();
	wait(1);
	leve_bras();
	wait(1);
	fermeture_pince();
	wait(1);
}

void Proto::down_arm() {	
	descente_bras();
	wait(1);
	ouverture_pince();
	wait(1);
}

void Proto::up_arm() {
	fermeture_pince();
	wait(2);
	leve_bras();
	wait(1);
}

bool Proto::obstructed() {
	bool out = false;
	if (gp2fr->read() > SEUIL_GP2) {
		out = true;
	}
	if (gp2fl->read() > SEUIL_GP2) {
		out = true;
	}
	return out;
}