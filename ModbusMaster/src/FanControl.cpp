/*
 * FanControl.cpp
 *
 *  Created on: 4. mar. 2020
 *      Author: Rasmus
 */

#include "FanControl.h"



FanControl::FanControl() : node(2), 	// Create modbus object that connects to slave id 2
		ControlWord(&node, 0), StatusWord(&node, 3),
		OutputFrequency(&node, 102), Current(&node, 103),
		Frequency(&node, 1){

	node.begin(9600); // set transmission rate - other parameters are set inside the object and can't be changed here

	// need to use explicit conversion since printf's variable argument doesn't automatically convert this to an integer
	printf("Status=%04X\n", (int)StatusWord); // for debugging

	ControlWord = 0x0406; // prepare for starting

	printf("Status=%04X\n", (int)StatusWord); // for debugging 1333

	while (!(StatusWord & 1)){sleep(50);} //wait for READY TO SWITCH ON

	printf("Status=%04X\n", (int)StatusWord); // for debugging 1333

	ControlWord = 0x047F; // set drive to start mode

	printf("Status=%04X\n", (int)StatusWord); // for debugging 1333

	while(!(StatusWord & (1 << 12 | 0b0111))){sleep(50);} //wait for OPERATION ENABLED
	// give converter some time to set up
	// note: we should have a startup state machine that check converter status and acts per current status
	//       but we take the easy way out and just wait a while and hope that everything goes well

	printf("Status=%04X\n", (int)StatusWord); // for debugging 1337
}

void FanControl::sleep(int ms){
	uint32_t count = systicks;
	while (systicks < count + ms){}
}

void FanControl::speedPercent(int speed0){
	int speed = speed0;
	if (speed < 0) speed = 0;
	else if (speed > 100) speed = 100;
	else speed = (float)speed / 100 * MAX_SPEED;
	setFrequency(speed);
}

void FanControl::speed(int speed){

}

bool FanControl::setFrequency(uint16_t freq){
	int result;
	int ctr;
	bool atSetpoint;
	const int delay = 500;

	Frequency = freq; // set motor frequency

	printf("Set freq = %d\n", freq/40); // for debugging

	// wait until we reach set point or timeout occurs
	ctr = 0;
	atSetpoint = false;
	do {
		sleep(delay);
		// read status word
		result = StatusWord;
		// check if we are at setpoint
		if (result >= 0 && (result & 0x0100)) atSetpoint = true;
		ctr++;
	} while(ctr < 20 && !atSetpoint);

	printf("Elapsed: %d\n", ctr * delay); // for debugging

	return atSetpoint;
}

FanControl::~FanControl() {
	// TODO Auto-generated destructor stub
}

