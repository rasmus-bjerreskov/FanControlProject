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
#include "morseencoder.h"
#include "ITMwrapper.h"
// TODO: insert other include files here

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
	Board_LED_Set(0, true);
#endif
#endif

	DigitalIoPin red(0, 25, false);
	DigitalIoPin A0(0, 8, false);
	red.write(false);
	morse_encoder mo(red, A0);
	mo.set_period(80);
	ITM_wrapper wr;

	//mo.send("sos");
	char s[81];
	int c;

	while(1){
		int i = 0;
		while((c = Board_UARTGetChar()) != '\r' && i != 80){
			if (c != EOF){
				s[i] = c;
				i++;
				Board_UARTPutChar(c);
			}
		}
		Board_UARTPutChar('\r');
		Board_UARTPutChar('\n');
		s[i] = '\0';
		wr.print(s);
		mo.send(s);
	}
	// Force the counter to be placed into memory
	volatile static int i = 0 ;
	// Enter an infinite loop, just incrementing a counter
	while(1) {
		i++ ;
		// "Dummy" NOP to allow source level single
		// stepping of tight while() loop
		__asm volatile ("nop");
	}
	return 0 ;
}
