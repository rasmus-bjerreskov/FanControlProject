/*
 * SDP600.cpp
 *
 *  Created on: 5. mar. 2020
 *      Author: Rasmus
 */

#include "SDP600.h"

SDP600::SDP600() :
i2c(cfg), txBuffPtr(0xF1), devAddr(0x40), pressure(0), alt_corr(0.95) {

}

uint8_t SDP600::getPressure(){
	i2c.transaction(devAddr, &txBuffPtr, 1, aptr, 3);
	pressure = (aptr[0] << 8) | aptr [1];
	uint8_t toReturn;
	toReturn = (float)pressure / (PR_SCALE * alt_corr);
	return toReturn;
}

SDP600::~SDP600() {
	// TODO Auto-generated destructor stub
}

