/*
 * RealTimeClock.h
 *
 *  Created on: 20. feb. 2020
 *      Author: Rasmus
 */

#include "Sleeper.h"
#include "Imutex.h"
#include <ctime>
#include <mutex>

#ifndef REALTIMECLOCK_H_
#define REALTIMECLOCK_H_

class RealTimeClock : public Sleeper {
public:
	RealTimeClock(int tick_hz);
	RealTimeClock(int tick_hz, tm* start);
	void getTime(tm* now);
	void tick();
	virtual ~RealTimeClock();

private:
	Imutex guard;
	volatile int counter;
	volatile int hour;
	volatile int min;
	volatile int sec;
};

#endif /* REALTIMECLOCK_H_ */
