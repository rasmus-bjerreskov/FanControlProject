/*
 * FanControl.h
 *
 *  Created on: 4. mar. 2020
 *      Author: Rasmus
 */
#include <cstring>
#include <cstdio>

#include "ModbusMaster.h"
#include "ModbusRegister.h"
#include "LpcUart.h"
extern volatile uint32_t systicks;
#ifndef FANCONTROL_H_
#define FANCONTROL_H_

class FanControl {
public:
	FanControl();
	void speedPercent(int speed);
	void speed(int speed);
	void sleep(int);
	bool setFrequency(uint16_t freq);
	virtual ~FanControl();

private:
	ModbusMaster node;
	ModbusRegister ControlWord;
	ModbusRegister StatusWord;
	ModbusRegister OutputFrequency;
	ModbusRegister Current;
	ModbusRegister Frequency; // reference 1
	static const int MAX_SPEED = 20000;


};

#endif /* FANCONTROL_H_ */
