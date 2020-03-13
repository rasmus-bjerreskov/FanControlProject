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
#include "ritimer_15xx.h"
#include "DigitalIoPin.h"
#include "LiquidCrystal.h"
#include "Sleeper.h"
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

	Chip_RIT_Init(LPC_RITIMER);
	Sleeper s;

	DigitalIoPin sw1(0, 17, true, true, true);
	DigitalIoPin sw2(1, 11, true, true, true);
	DigitalIoPin sw3(1, 9, true, true, true);

	DigitalIoPin rs(0, 8, false, false);
	DigitalIoPin en(1, 6, false, false);
	DigitalIoPin d4(1, 8, false, false);
	DigitalIoPin d5(0, 5, false, false);
	DigitalIoPin d6(0, 6, false, false);
	DigitalIoPin d7(0, 7, false, false);

	LiquidCrystal lcd(&rs, &en, &d4, &d5, &d6, &d7);
	// configure display geometry
	lcd.begin(16, 2);
	// set the cursor to column 0, line 1
	// (note: line 1 is the second row, since counting begins with 0):


	bool B1 = false, B2 = false, B3 = false;
	lcd.setCursor(0, 0);
	lcd.print("B1   B2   B3");

	while (1){
		lcd.setCursor(0, 0);
		if (sw1.read())
			B1 = true;
		else
			B1 = false;

		if (sw2.read())
			B2 = true;
		else
			B2 = false;

		if (sw3.read())
			B3 = true;
		else
			B3 = false;

		lcd.setCursor(0, 1);
		if(B1)
			lcd.print("Down");
		else
			lcd.print("Up  ");

		lcd.setCursor(5, 1);
		if(B2)
			lcd.print("Down");
		else
			lcd.print("Up  ");

		lcd.setCursor(10, 1);
		if(B3)
			lcd.print("Down");
		else
			lcd.print("Up  ");




		s.sleep(1);
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
