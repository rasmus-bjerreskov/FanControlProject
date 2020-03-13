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

// TODO: insert other include files here
#include <cctype>
#include "ITMwrapper.h"
#include "DigitalIoPin.h"
#include "cstm_sleep.h"
// TODO: insert other definitions and declarations here
#define TICKRATE_HZ1 (1000)

#include <cr_section_macros.h>

#include <atomic>
static volatile std::atomic_int counter;

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Handle interrupt from SysTick timer
 * @return Nothing
 */
void SysTick_Handler(void)
{
	if(counter > 0) counter--;
}
#ifdef __cplusplus
}
#endif
void sleep(int ms)
{
	counter = ms;
	while(counter > 0) {
		__WFI();
	}
}
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

	uint32_t sysTickRate;

	Chip_Clock_SetSysTickClockDiv(1);
	sysTickRate = Chip_Clock_GetSysTickClockRate();
	SysTick_Config(sysTickRate / TICKRATE_HZ1);

	Board_UARTPutSTR("\r\nHello World\r\n");
	Board_UARTPutChar('!');
	Board_UARTPutChar('\r');
	Board_UARTPutChar('\n');

	ITM_wrapper w;
	DigitalIoPin sw1(0, 17, true, true, true);
	DigitalIoPin green(0, 3, false);

	int c;
	bool upper = false;
	while(1){
		//button toggle
		if (sw1.read()){
			upper = !upper;
			green.write(upper);
			while (sw1.read())
				sleep(1);
		}
		c = Board_UARTGetChar();
		if(c != EOF){
			if(upper) c = toupper(c);
			else c = tolower(c);
			Board_UARTPutChar(c);
			if(c == '\r') Board_UARTPutChar('\n');
			w.print(c); //for debug
			w.print(" ");
		}

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
