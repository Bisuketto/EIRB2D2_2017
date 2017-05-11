#include "PathFinderCom.hpp"

PathFinderCom::PathFinderCom()
{
	raspCom = new Serial(PIN_COMRASP_TX, PIN_COMRASP_RX, 9600);
}

PathFinderCom::~PathFinderCom()
{
}

std::vector<float> PathFinderCom::getNext(float x_dest, float y_dest, float xi, float yi) {
	std::vector<float> pt_out;
	raspCom->printf("");
	char buff[64];
	raspCom->gets(buff, 64);
	char instr[64] = "";
	int i = 0;
	while ((buff[i] != '~') && (buff[i] != '\0') && (i < 64)) {
		instr[i] = buff[i];
		i++;
	}
	
	if (strcmp(instr, "n_pt") == 0) {
		char op[64] = "";
		float x;
		i++;
		int k = i;
		while ((buff[i] != '~') && (buff[i] != '\0') && (i < 64)) {
			op[i - k] = buff[i];
			i++;
		}
		x = atof(op);
		for (int j = 0; j < 64; j++) {
			op[j] = 0;
		}
		float y;
		i++;
		k = i;
		while ((buff[i] != '\n') && (buff[i] != '\0') && (i < 64)) {
			op[i - k] = buff[i];
			i++;
		}
		y = atof(op);

		pt_out.push_back(x);
		pt_out.push_back(y);
		
	}
	return pt_out;
}