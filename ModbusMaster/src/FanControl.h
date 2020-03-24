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

/*Status word bits*/
#define RDY_ON 				1 << 0
#define RDY_RUN 			1 << 1
#define RDY_REF 			1 << 2
#define TRIPPED 			1 << 3
#define OFF_2_STA 			1 << 4
#define OFF_3_STA 			1 << 5
#define SWC_ON_INHIB 		1 << 6
#define ALARM 				1 << 7
#define AT_SETPOINT 		1 << 8
#define REMOTE 				1 << 9
#define ABOVE_LIMIT			1 << 10
#define EXT_CTRL_LOC 		1 << 11
#define EXT_RUN_ENABLE 		1 << 12

/*Control word bits*/
#define OFF1_CONTROL 		1 << 0
#define OFF2_CONTROL 		1 << 1
#define OFF3_CONTROL 		1 << 2
#define INHIBIT_OPERATION 	1 << 3
#define RAMP_OUT_ZERO 		1 << 4
#define RAMP_HOLD 			1 << 5
#define RAMP_IN_ZERO 		1 << 6
#define RESET 				1 << 7
#define REMOTE_CMD 			1 << 10
#define EXT_CTRL_LOC 		1 << 11

extern volatile uint32_t systicks;
#ifndef FANCONTROL_H_
#define FANCONTROL_H_

class FanControl {
public:
	FanControl();
	void speedPercent(uint8_t speed);
	bool setFrequency(uint16_t freq);
	uint16_t getSpeed();
	uint16_t tToSetpoint();
	void updateFrequency();
	void setTarget(uint16_t freq);
	uint16_t getTarget();
	virtual ~FanControl();

private:
	ModbusMaster node;
	ModbusRegister ControlWord;
	ModbusRegister StatusWord;
	ModbusRegister OutputFrequency;
	ModbusRegister Current;
	ModbusRegister Frequency; // reference 1
	static const int MAX_SPEED = 20000;
	void sleep(int);
	uint16_t ctr;
	uint16_t targetSpeed;

};

#endif /* FANCONTROL_H_ */
