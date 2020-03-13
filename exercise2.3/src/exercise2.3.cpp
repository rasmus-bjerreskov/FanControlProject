/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
 */

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>
#include <algorithm>
#include "ITMwrapper.h"
#include "DigitalIoPin.h"
#include "Sleeper.h"
// TODO: insert other include files here

#define PRESSES 30
#define CYCLES_PR_uS 72
// TODO: insert other definitions and declarations here

int main(void) {

#if defined (__USE_LPCOPEN)
	// Read clock settings and update SystemCoreClock variable

	SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
	// Set up and initialize all required blocks and
	// functions related to the board hardware
	Board_Init();
	// Set the LED to the state of "On"
	//Board_LED_Set(0, true);
#endif
#endif
	ITM_wrapper iw;
	DigitalIoPin sw1(0, 17, true, true, true);
	Sleeper s;
	char str[80] = {};
	uint8_t i = 0;
	uint32_t log[PRESSES] = {};
	uint32_t sum = 0;
	CoreDebug->DEMCR |= 1 << 24;
	DWT->CTRL |= 1;

	uint32_t value;

	while (i < PRESSES){
		if (sw1.read()){
			value = DWT->CYCCNT;
			s.sleep(1);
			log[i] = DWT->CYCCNT - value;
			i++;
			iw.print(i);
			iw.print("\n");

			while(sw1.read());
			s.sleep(50);
		}


	}

	std::sort(log, log + 30);
	for(i = 0; i < 30; ++i){
		sum += log[i];
		sprintf(str, "%d: %lu cycles, %lu us\n", i+1, log[i], log[i] / CYCLES_PR_uS);
		iw.print(str);
	}

	int avgc = (float)sum / PRESSES;
	int avgt = (float)sum / (PRESSES * CYCLES_PR_uS);
	sprintf(str, "\nAvg cycles: %d\nAvg time: %d us\n", avgc, avgt);
	iw.print(str);
	// Force the counter to be placed into memory
	//volatile static int i = 0 ;
	// Enter an infinite loop, just incrementing a counter
	while(1) {
		s.sleep(20);
		// "Dummy" NOP to allow source level single
		// stepping of tight while() loop
		__asm volatile ("nop");
	}
	return 0 ;
}
