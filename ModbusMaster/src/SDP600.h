/*
 * SDP600.h
 *
 *  Created on: 5. mar. 2020
 *      Author: Rasmus
 */

#include "I2C.h"

#ifndef SDP600_H_
#define SDP600_H_

class SDP600 {
public:
	SDP600();
	uint8_t getPressure();
	virtual ~SDP600();

private:
	I2C_config cfg;
	I2C i2c;
	uint8_t aptr[3];
	uint8_t txBuffPtr;
	const uint8_t devAddr;
	int16_t pressure;
	static const int PR_SCALE = 240;
	const float alt_corr;
};

#endif /* SDP600_H_ */
