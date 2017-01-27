#include "UIcom.hpp"

UIcom::UIcom(){
	erreur_d = 0;
	erreur_g = 0;
	pwmg = 0;
	pwmd = 0;
	dist_g = 0;
	dist_d = 0;
	current_g = 0;
	current_d = 0;
	bat9 = 0;
	bat18 = 0;
	serialCom = new Serial(UI_SERIAL_TX, UI_SERIAL_RX, UI_SERIAL_BAUDRATE);
	schedule = new Ticker;
	schedule->attach(callback(this, &UIcom::send_Infos), PERIODE_UI);
}

void UIcom::send_Infos() {
	serialCom->printf("infos~%f~%f~%f~%f~%f~%f~%f~%f~%f~%f\n", erreur_d, erreur_g, pwmd, pwmg, dist_d, dist_g, current_d, current_g, bat9, bat18);
}

void UIcom::set_erreur_d(float value) {
	erreur_d = value;
}

void UIcom::set_erreur_g(float value) {
	erreur_g = value;
}

void UIcom::set_pwmd(float value) {
	pwmd = value;
}

void UIcom::set_pwmg(float value) {
	pwmg = value;
}

void UIcom::set_dist_d(float value) {
	dist_d = value;
}

void UIcom::set_dist_g(float value) {
	dist_g = value;
}

void UIcom::set_current_d(float value) {
	current_d = value;
}

void UIcom::set_current_g(float value) {
	current_g = value;
}

void UIcom::set_bat9(float value) {
	bat9 = value;
}

void UIcom::set_bat18(float value) {
	bat18 = value;
}

UIcom::~UIcom()
{
}
