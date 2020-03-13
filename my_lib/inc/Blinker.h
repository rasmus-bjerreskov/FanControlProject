/*
 * Blinker.h
 *
 *  Created on: 12. feb. 2020
 *      Author: Rasmus
 */

#ifndef BLINKER_H_
#define BLINKER_H_

#include <mutex>
#include "Sleeper.h"
#include "DigitalIoPin.h"
#include "Imutex.h"

class Blinker : public Sleeper {
public:
	Blinker();
	virtual void tick();
	void blink(DigitalIoPin*, int);
	virtual ~Blinker();

private:
	DigitalIoPin* act;
	Imutex guard;
	int limit;
	volatile std::atomic_int counter;
	bool state;
};

#endif /* BLINKER_H_ */
