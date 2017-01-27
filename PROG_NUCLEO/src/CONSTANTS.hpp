#pragma once

#define PERIODE_ENCODER 0.000061038881767686015992187023133736 //Intervalle de recupération à 16383Hz
#define RESOLUTION 4096 // Mecanical parameters
#define PERIMETER 87.96
#define SPEED_CONSTANT 393 //rpm/V
#define RAPPORT_ROUX 2.1428
#define VITESSE_MAX_MOTEUR 22.73
#define INC_MAX 199498  //37782
#define TAILLE_TABLEAUX 4
#define PERIODE_ASSERV 0.0009765625 //Intervalle d'échantillonnage à 1024Hz
#define PERIODE_PWM 25 //µs : 40kHz
#define UI_SERIAL_TX USBTX
#define UI_SERIAL_RX USBRX
#define UI_SERIAL_BAUDRATE 9600
#define PERIODE_UI 0.500
#define PERIODE_SEND_UI 0.500