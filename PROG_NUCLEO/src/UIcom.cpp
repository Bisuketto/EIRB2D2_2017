#include "UIcom.hpp"

UIcom::UIcom(Motor *instMotor){
	status = false;
	status_motor = false;
	ptomotor = instMotor;
	erreur_pos = 0;
	erreur_ang = 0;
	pwmg = 0;
	pwmd = 0;
	dist_g = 0;
	dist_d = 0;
	current_g = 0;
	current_d = 0;
	bat9 = 0;
	bat18 = 0;
	theta = 0;
	x = 0;
	y = 0;
	t = 0;
	omega = 0;
	serialCom = new Serial(PD_5, PD_6, 57600); //new Serial(PG_14, PG_9, 57600);//new Serial(UI_SERIAL_TX, UI_SERIAL_RX, UI_SERIAL_BAUDRATE);
	scheduleOut = new Ticker;
	scheduleOut->attach(callback(this, &UIcom::send_Infos), PERIODE_UI_OUT);
	serialCom->attach(callback(this, &UIcom::get_instr));
}

void UIcom::get_instr() {
	if (!status) {
		status = true;
		char buff[64] = "";
		serialCom->gets(buff, 64);
		status = false;
		char instr[64] = "";
		int i = 0;
		while ((buff[i] != '~') && (buff[i] != '\0') && (i < 64)) {
			instr[i] = buff[i];
			i++;
		}
		serialCom->printf(instr);
		serialCom->printf("\n");
		if (strcmp(instr, "stop") == 0) {
			ptomotor->stop();
			serialCom->printf("Stop re�u\n");
		}
		else if (strcmp(instr, "cpos") == 0) {
			char op[64] = "";
			float consigne_pos;
			i++;
			int k = i;
			while ((buff[i] != '\n') && (buff[i] != '\0') && (i < 64)) {
				op[i-k] = buff[i];
				i++;
			}
			op[i - k] = buff[i];
			//for (int i = 0; i < 64; i++)
			//	serialCom->printf("%d", op[i]);
			consigne_pos = atof(op);
			for (int j = 0; j < 64; j++) {
				op[j] = 0;
			}
			if (ptomotor->get_state() == true)
				serialCom->printf("Busy\n");
			//while (ptomotor->get_state() == true);
			ptomotor->position(consigne_pos, 0);
			//while (ptomotor->get_state() == true);
		}
		else if (strcmp(instr, "cang") == 0) {
			char op[64] = "";
			float consigne_ang;
			i++;
			int k = i;
			while ((buff[i] != '\n') && (buff[i] != '\0') && (i < 64)) {
				op[i - k] = buff[i];
				i++;
			}
			op[i - k] = buff[i];
			//for (int i = 0; i < 64; i++)
			//	serialCom->printf("%d", op[i]);
			consigne_ang = atof(op);
			for (int j = 0; j < 64; j++) {
				op[j] = 0;
			}
			if (ptomotor->get_state() == true)
				serialCom->printf("Busy\n");
			//while (ptomotor->get_state() == true);
			ptomotor->position(0, consigne_ang*PI/180);
			//while (ptomotor->get_state() == true);
		}
		else if (strcmp(instr, "cspd") == 0){
			serialCom->printf("CSPD!\n");
			char op[64] = "";
			float consigne_vg;
			i++;
			int k = i;
			while ((buff[i] != '~') && (buff[i] != '\0') && (i < 64)) {
				op[i - k] = buff[i];
				i++;
			}
			consigne_vg = atof(op);
			serialCom->printf("%f\n", consigne_vg);
			for (int j = 0; j < 64; j++) {
				op[j] = 0;
			}
			float consigne_vd;
			i++;
			k = i;
			while ((buff[i] != '\n') && (buff[i] != '\0') && (i < 64)) {
				op[i - k] = buff[i];
				i++;
			}
			consigne_vd = atof(op);
			serialCom->printf("%f\n", consigne_vd);
			ptomotor->vitesse_man(consigne_vg, consigne_vd);
			serialCom->printf("Cspd finished\n");
		}
	}
}

void UIcom::sendText(char *str) {
	serialCom->printf(str);
}

void UIcom::send_Infos() {
	serialCom->printf("infos~%5.1f~%5.1f~%1.4f~%1.4f~%5.1f~%5.1f~%2.3f~%2.3f~%2.2f~%2.2f~%5.2f~%5.2f~%5.2f~%5.2f~%5.2f~%d\n", erreur_pos, erreur_ang, pwmd, pwmg, dist_d, dist_g, current_d, current_g, bat9, bat18, x, y, theta, t, omega, status_motor);
}

void UIcom::set_erreur_pos(float value) {
	erreur_pos = value;
}

void UIcom::set_erreur_ang(float value) {
	erreur_ang = value;
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

void UIcom::set_status_motor(bool value) {
	status_motor = value;
}

void UIcom::set_pos(float xi, float yi, float thetai) {
	x = xi;
	y = yi;
	theta = thetai;
}

void UIcom::set_tinfo(float ti, float omegai) {
	t = ti;
	omega = omegai;
}

UIcom::~UIcom()
{
}
