#include "UIcom.hpp"

UIcom::UIcom(){
	serialCom = new Serial(UI_SERIAL_TX, UI_SERIAL_RX, UI_SERIAL_BAUDRATE);
	schedule = new Ticker(callback(this, *UIcom::setInfos), PERIODE_UI);
}

void UIcom::setInfos() {

}

UIcom::~UIcom()
{
}
