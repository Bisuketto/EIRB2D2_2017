#pragma once

#define PERIODE_ENCODER (1./16383)
#define RESOLUTION 4096 // Mecanical parameters
#define PERIMETER 87.96
#define SPEED_CONSTANT 393 //rpm/V
#define RAPPORT_ROUX 2.1428
#define VITESSE_MAX 37782
#define KP 0.707107
#define KD 0
#define KI 0
#define PERIODE_ASSERV (1./512) //Intervalle d'échantillonnage à 256Hz
#define PERIODE_PWM 25 //µs