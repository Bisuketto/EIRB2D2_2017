#include "Encoders.hpp"

Encoders::Encoders() {
	modeTest = false;
	impEncD = 0;
	impEncG = 0;
	TIM3_EncoderInit();
	TIM4_EncoderInit();
	scheduler_Encoders = new Ticker;
	scheduler_Encoders->attach(callback(this, &Encoders::routine_Encoders), PERIODE_ENCODER);//16383Hz
	x = 0;
	y = 0;
	theta0 = 0;
}

Encoders::Encoders(float x0, float y0, float theta_i) {
	modeTest = false;
	impEncD = 0;
	impEncG = 0;
	TIM3_EncoderInit();
	TIM4_EncoderInit();
	scheduler_Encoders = new Ticker;
	scheduler_Encoders->attach(callback(this, &Encoders::routine_Encoders), PERIODE_ENCODER);//16383Hz
	dtot_g = 0;
	dtot_d = 0;
	x = x0;
	y = y0;
	theta0 = theta_i;
}

Encoders::Encoders(Serial *pcOut) {
	modeTest = true;
	pc = pcOut;
	impEncD = 0;
	impEncG = 0;
	TIM3_EncoderInit();
	TIM4_EncoderInit();
	scheduler_Encoders = new Ticker;
	scheduler_Encoders->attach(callback(this, &Encoders::routine_Encoders), PERIODE_ENCODER);//100Hz
	x = 0;
	y = 0;
	theta0 = 0;
}

int Encoders::getImpEncD()
{
	int i = impEncD;
	impEncD = 0;
	return i;
}

int Encoders::getImpEncG()
{
	int i = impEncG;
	impEncG = 0;
	return i;
}

void Encoders::routine_Encoders()
{
	int g = (TIM3->CNT - (0xFFFF / 2));
	TIM3->CNT = (0xFFFF / 2);
	int d = (TIM4->CNT - (0xFFFF / 2));
	TIM4->CNT = (0xFFFF / 2);

	impEncG += g;
	impEncD += d;

	dld = d; // in imp.s-1
	dlg = g;
	dtot_d += d;
	dtot_g += g;
	odometrie();
}

void Encoders::odometrie() {
	theta = theta0 + ((dtot_d - dtot_g)*PERIMETER / RESOLUTION)/(2*RADIUS_ENC);
	dl = ((dld + dlg)*PERIMETER / RESOLUTION) / 2;

	x += -dl*sin(theta);
	y +=  dl*cos(theta);
}

float Encoders::getX() {
	return x;
}

float Encoders::getY() {
	return y;
}

float Encoders::getTheta() {
	return theta;
}

float Encoders::getDd() {
	return dtot_d;
}

float Encoders::getDg() {
	return dtot_g;
}

void Encoders::TIM3_EncoderInit() {
	__GPIOA_CLK_ENABLE();
	__TIM3_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	TIM3->SMCR = 3;          // Encoder mode 3
	TIM3->CCER = 0;          // rising edge polarity
	TIM3->CCER |= (1 << 1);
	TIM3->ARR = 0xFFFF;      // count from 0-ARR or ARR-0
	TIM3->CCMR1 = 0xC1C1;    // f_DTS/16, N=8, IC1->TI1, IC2->TI2
	TIM3->EGR = 1;           // Generate an update event
	TIM3->CR1 = 1;           // Enable the counter
	TIM3->CNT = (0xFFFF / 2);           // Initialize counter
}

void Encoders::TIM4_EncoderInit() {
	GPIO_InitTypeDef GPIO_InitStruct;

	__GPIOD_CLK_ENABLE();
	__TIM4_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	TIM4->SMCR = 3;          // Encoder mode 3
	TIM4->CCER = 0;          // rising edge polarity
	TIM4->ARR = 0xFFFF;      // count from 0-ARR or ARR-0
	TIM4->CCMR1 = 0xC1C1;    // f_DTS/16, N=8, IC1->TI1, IC2->TI2
	TIM4->EGR = 1;           // Generate an update event
	TIM4->CR1 = 1;           // Enable the counter
	TIM4->CNT = (0xFFFF / 2);          // Initialize counter
}

Encoders::~Encoders()
{
}
