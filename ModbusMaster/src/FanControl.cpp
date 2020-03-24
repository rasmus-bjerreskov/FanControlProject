/*
 * FanControl.cpp
 *
 *  Created on: 4. mar. 2020
 *      Author: Rasmus
 */

#include "FanControl.h"
#define DEBUG_FAN


FanControl::FanControl() : node(2), 	// Create modbus object that connects to slave id 2
		ControlWord(&node, 0), StatusWord(&node, 3),
		OutputFrequency(&node, 102), Current(&node, 103),
		Frequency(&node, 1){

	node.begin(9600); // set transmission rate - other parameters are set inside the object and can't be changed here

	// need to use explicit conversion since printf's variable argument doesn't automatically convert this to an integer
	printf("Status=%04X\n", (int)StatusWord); // for debugging

	ControlWord = (REMOTE_CMD | OFF3_CONTROL | OFF2_CONTROL); // prepare for starting

	printf("Status=%04X\n", (int)StatusWord); // for debugging 1333

	while (!(StatusWord & RDY_ON)){sleep(2);} //wait for READY TO SWITCH ON

	printf("Status=%04X\n", (int)StatusWord); // for debugging 1333

	ControlWord = (REMOTE_CMD | RAMP_IN_ZERO | RAMP_HOLD | RAMP_OUT_ZERO | INHIBIT_OPERATION
			| OFF3_CONTROL | OFF2_CONTROL | OFF1_CONTROL); // set drive to start mode

	printf("Status=%04X\n", (int)StatusWord); // for debugging 1333

	while(!(StatusWord & (EXT_RUN_ENABLE | RDY_REF | RDY_RUN | RDY_ON))){sleep(2);} //wait for OPERATION ENABLED
	// give converter some time to set up
	// note: we should have a startup state machine that check converter status and acts per current status
	//       but we take the easy way out and just wait a while and hope that everything goes well

	printf("Status=%04X\n", (int)StatusWord); // for debugging 1337
	speedPercent(0);
	ctr = 0;
	targetSpeed = 0;
}

void FanControl::sleep(int ms){
	uint32_t count = systicks;
	while (systicks < count + ms){}
}

void FanControl::speedPercent(uint8_t speed){
	if (speed > 100) speed = 100;
	setFrequency((float)speed / 100 * MAX_SPEED);
}

bool FanControl::setFrequency(uint16_t freq){
	int result;
	int ctr;
	bool atSetpoint;
	const int delay = 50;

	Frequency = (freq > MAX_SPEED) ? MAX_SPEED : freq;
#ifdef DEBUG_FAN
	printf("Set freq = %d\n", freq/40); // for debugging
#endif
	// wait until we reach set point or timeout occurs
	ctr = 0;
	atSetpoint = false;
	do {
		sleep(delay);
		// read status word
		result = StatusWord;
		// check if we are at setpoint
		if (result & AT_SETPOINT) atSetpoint = true;
		ctr++;
	} while(ctr < 10 && !atSetpoint);
#ifdef DEBUG_FAN
	printf("Elapsed: %d\n", ctr * delay); // for debugging
#endif
	return atSetpoint;
}

uint16_t FanControl::getSpeed(){
	return OutputFrequency;
}

void FanControl::updateFrequency(){
	setFrequency(targetSpeed);
}

void FanControl::setTarget(uint16_t freq){
	targetSpeed = (float)freq /100 * 20000;
}

uint16_t FanControl::getTarget(){
	return targetSpeed;
}

FanControl::~FanControl() {
	// TODO Auto-generated destructor stub
}

