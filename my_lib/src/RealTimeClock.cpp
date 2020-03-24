/*
 * RealTimeClock.cpp
 *
 *  Created on: 20. feb. 2020
 *      Author: Rasmus
 */

#include <RealTimeClock.h>

RealTimeClock::RealTimeClock(int tick_hz) : Sleeper(tick_hz) {
	hour = 53;
	min = 59;
	sec = 35;
	counter = 0;
}

RealTimeClock::RealTimeClock(int tick_hz, tm* start) {
	hour = start->tm_hour;
	min = start->tm_min;
	sec = start->tm_sec;
	counter = 0;
}

void RealTimeClock::getTime(tm* now){
	std::lock_guard<Imutex> lock(guard);
	now->tm_hour = hour;
	now->tm_min = min;
	now->tm_sec = sec;
}

void RealTimeClock::tick(){
	Sleeper::tick();
	++counter;
	if(counter == 1000){
		counter = 0;
		++sec;
		if (sec == 60){
			sec = 0;
			++min;
			if (min == 60){
				min = 0;
				++hour;
				if (hour == 24){
					hour = 0;
				}
			}
		}
	}
}

RealTimeClock::~RealTimeClock() {
	// TODO Auto-generated destructor stub
}

