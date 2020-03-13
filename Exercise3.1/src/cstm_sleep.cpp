/*
 * cust_sleep.cpp
 *
 *  Created on: 28. jan. 2020
 *      Author: Rasmus
 */
#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

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

static void sleep_for(int ms){
//copied from blinky systick.c
	int init = 0;

	if (!init){

	uint32_t sysTickRate;

	Chip_Clock_SetSysTickClockDiv(1);
	sysTickRate = Chip_Clock_GetSysTickClockRate();
	SysTick_Config(sysTickRate / TICKRATE_HZ1);
	init = 1;
	}

	sleep(ms);
}
