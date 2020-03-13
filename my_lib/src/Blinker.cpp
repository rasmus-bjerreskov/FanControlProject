/*
 * Blinker.cpp
 *
 *  Created on: 12. feb. 2020
 *      Author: Rasmus
 */

#include "Blinker.h"

Blinker::Blinker(){
	state = false;
	counter = -1;
	limit = 0;
	act = nullptr;
}

void Blinker::tick(){
	Sleeper::tick();
	if (limit > 0){
		counter++;

		if (counter >= limit){
			state = !state;
			act->write(state);
			counter = 0;
		}
	}
}

void Blinker::blink(DigitalIoPin* cur, int val){
	std::lock_guard<Imutex> lock(guard);
	if (cur != act){
		//turn off previous led
		if (act != nullptr)
			act->write(false);
		act = cur;
		act->write(true);
	}
	limit = val;
}

Blinker::~Blinker() {
	// TODO Auto-generated destructor stub
}

