#pragma once

#include "mbed.h"
#include "CONSTANTS.hpp"
#include "PINOUT_NUCLEO.hpp"
#include "Encoders.hpp"
#include "UIcom.hpp"
#include <string>
#include <stdlib.h>

class UIcom;

class Motor
{
	public:
		Motor();
		Motor(Serial *pc_out);
		void link_to_ui(UIcom *ui);
		void stop();
		void vitesse(float vitesse); //Speed in mm/s
		void vitesse_man(float vg, float vd);
		void trajectoire(float vitesse);
		void position(float distance, float angle); //Distance to travel in mm
		void angle(float angle); //Angle to rotate in degrees
		void asserv_trajectoire();
		void asserv_vitesse();
		void asserv_position();
		void calc_sens(float vd, float vg);
		void calc_vitesse();
		void debug(int* reading);
		void consigne_vd_change(float consigne);
		void consigne_vg_change(float consigne);
		void consigne_a_change(float consigne);
		void consigne_p_change(float consigne);
		bool get_state();
		~Motor();
	private:
		void push_in_tab(float x, float tableau[]);
		void send_to_ui();
		float consigne_hyperbolique();
		UIcom *interfaceCom;
		Encoders *instEncoder;
		Ticker *routineAsserv;
		Ticker *refreshUI;
		Timer *tc;
		Timer *affichage_debug;
		Serial *pc;
		std::string buff_in;
		bool status; // True if busy
		bool hasToStop; //Stop if True
		bool modeTest;
		PwmOut *motorD;
		PwmOut *motorG;
		DigitalIn * bouton;
		DigitalOut *sens_mD;
		DigitalOut *sens_mG;
		int sensG;
		int sensD;
		float consigne_vitesse_d; //In Imp.s^-1
		float consigne_vitesse_g;
		float consigne_vitesse;
		float consigne_angle;
		float consigne_position; //In Imp
		double vitesse_d; //In Imp.s^-1
		double vitesse_g; //In Imp.s^-1
		int imp_d; //Right Encoder Impulsions Count
		int imp_g; //Left Encoder Impulsions Count
		float pwmd; //Right Motor PWM
		float pwmg; //Left Motor PWM
		float pwms_d[TAILLE_TABLEAUX];
		float pwms_g[TAILLE_TABLEAUX];
		float errors_d[TAILLE_TABLEAUX];
		float errors_g[TAILLE_TABLEAUX];
		float errors_angle[TAILLE_TABLEAUX];
		float errors_dist[TAILLE_TABLEAUX];
		float consignes_v[TAILLE_TABLEAUX];
		float consignes_a[TAILLE_TABLEAUX];
		int dist_d;
		int dist_g;
		float last_change_d;
};