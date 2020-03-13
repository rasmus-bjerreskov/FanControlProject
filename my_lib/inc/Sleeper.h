/*
 * Sleeper.h
 *
 *  Created on: 29. jan. 2020
 *      Author: Rasmus
 */

#ifndef SLEEPER_H_
#define SLEEPER_H_
#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif
#include <cr_section_macros.h>
#include <atomic>

class Sleeper {
public:
	Sleeper();
	Sleeper(int);
	void sleep(int);
	virtual void tick();
	virtual ~Sleeper();

private:
	const int TICKRATE_HZ1;
	volatile std::atomic_int counter;
};

#endif /* SLEEPER_H_ */
