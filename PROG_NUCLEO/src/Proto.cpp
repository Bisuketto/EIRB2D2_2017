#include "Proto.hpp"

Proto::Proto()
{
	bras = new PwmOut(PIN_BRAS);
	pince = new PwmOut(PIN_PINCE);
	leve_bras();
	wait(1);
	fermeture_pince();
	wait(1);
}


Proto::~Proto()
{
}

void Proto::fermeture_pince() {
	for (int p = 2400; p > 1400; p -= 50) { //1500
		pince->pulsewidth_us(p);
		wait(0.01);
	}
}

void Proto::ouverture_pince() {
	for (int p = 1500; p < 2400; p += 50) {
		pince->pulsewidth_us(p);
		wait(0.01);
	}
}

void Proto::descente_bras() {
	for (int p = 1500; p < 2150; p += 50) {
		bras->pulsewidth_us(p);
		wait(0.01);
	}
}

void Proto::leve_bras() {
	for (int p = 2400; p > 1200; p -= 50) { //1100
		bras->pulsewidth_us(p);
		wait(0.01);
	}
}

void Proto::recup() {
	ouverture_pince();
	wait(1);
	descente_bras();
	wait(1);
	fermeture_pince();
	wait(1);
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
	wait(1);
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