#pragma once

#define PERIODE_ENCODER (1./16383)
#define RESOLUTION 4096 // Mecanical parameters
#define PERIMETER 87.96
#define SPEED_CONSTANT 393 //rpm/V
#define RAPPORT_ROUX 2.1428
#define VITESSE_MAX 37782
#define TAILLE_TABLEAUX 4
#define PERIODE_ASSERV (1./1024) //Intervalle d'échantillonnage à 256Hz
#define PERIODE_PWM 25 //µs
#define UI_SERIAL_TX USBTX
#define UI_SERIAL_RX USBRX
#define UI_SERIAL_BAUDRATE 9600
#define PERIODE_UI 1