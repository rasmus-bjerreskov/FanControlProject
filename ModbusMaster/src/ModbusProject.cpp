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

// TODO: insert other definitions and declarations here


#include <cstring>
#include <cstdio>
#include <atomic>

#include "ring_buffer.h"
#include "LiquidCrystal.h"
#include "Menu.h"
#include "SDP600.h"
#include "FanControl.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/


/*****************************************************************************
 * Public functions
 ****************************************************************************/
//#define BTN_DEBUG

#define SW1_PORT 0 	//"BACK"
#define SW1_PIN 10 	//"BACK"
#define SW2_PORT 0 	//"UP"
#define SW2_PIN 16 	//"UP"
#define SW3_PORT 1 	//"OK"
#define SW3_PIN 3  	//"OK"
#define SW4_PORT 0 	//"DOWN"
#define SW4_PIN 0  	//"DOWN

#define BOUNCE_TIMER 200 //Min time between accepted edge input - these buttons are not amazing

Menu* menu;
static RINGBUFF_T rBuf;
static Event events[20];

volatile uint32_t systicks;
static volatile int sleep_count(0);
static volatile int timeout_count(0);


#ifdef __cplusplus
extern "C" {
#endif

void SysTick_Handler(void){
	++systicks;
	++sleep_count;
	if (systicks % 100 == 0){
		Event e;
		RingBuffer_Insert(&rBuf, (const void*)&e);
	}
}

void PIN_INT0_IRQHandler(void)
{
	static uint32_t last_count;
	if ((systicks - last_count) >= BOUNCE_TIMER){
		Event e(Event::back);
		RingBuffer_Insert(&rBuf, (const void*)&e);;
		last_count = systicks;
#ifdef BTN_DEBUG
		printf("Sw3 pressed\n");
#endif
	}
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0));
}

void PIN_INT1_IRQHandler(void)
{
	static uint32_t last_count;
	if ((systicks - last_count) >= BOUNCE_TIMER){
		Event e(Event::up);
		RingBuffer_Insert(&rBuf, (const void*)&e);
		last_count = systicks;
#ifdef BTN_DEBUG
		printf("Sw2 pressed\n");
#endif
	}
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(1));
}

void PIN_INT2_IRQHandler(void)
{
	static uint32_t last_count;
	if ((systicks - last_count) >= BOUNCE_TIMER){
		Event e(Event::ok);
		RingBuffer_Insert(&rBuf, (const void*)&e);;
		last_count = systicks;
#ifdef BTN_DEBUG
		printf("Sw3 pressed\n");
#endif
	}
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(2));
}

void PIN_INT3_IRQHandler(void)
{
	static uint32_t last_count;
	if ((systicks - last_count) >= BOUNCE_TIMER){
		Event e(Event::down);
		RingBuffer_Insert(&rBuf, (const void*)&e);
		last_count = systicks;
#ifdef BTN_DEBUG
		printf("Sw4 pressed\n");
#endif
	}
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(3));
}

#ifdef __cplusplus
}
#endif

void sleep(int target){
	sleep_count = 0;
	while (sleep_count < target)
		__WFI();
}

/* this function is required by the modbus library */
uint32_t millis() {
	return systicks;
}

/**
 * @brief	Main UART program body
 * @return	Always returns 1
 */
int main(void)
{

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

	/* Enable and setup SysTick Timer at a periodic rate */
	SysTick_Config(SystemCoreClock / 1000);


	LpcPinMap none = {-1, -1}; // unused pin has negative values in it
	LpcPinMap txpin = { 0, 18 }; // transmit pin that goes to debugger's UART->USB converter
	LpcPinMap rxpin = { 0, 13 }; // receive pin that goes to debugger's UART->USB converter
	LpcUartConfig cfg = { LPC_USART0, 115200, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1, false, txpin, rxpin, none, none };
	LpcUart dbgu(cfg);


	/* Set up SWO to PIO1_2 */
	Chip_SWM_MovablePortPinAssign(SWM_SWO_O, 1, 2); // Needed for SWO printf

	RingBuffer_Init(&rBuf, events, sizeof(Event), 20);

	DigitalIoPin SW1(SW1_PORT, SW1_PIN, true, true, true);
	DigitalIoPin SW2(SW2_PORT, SW2_PIN, true, true, true);
	DigitalIoPin SW3(SW3_PORT, SW3_PIN, true, true, true);
	DigitalIoPin SW4(SW4_PORT, SW4_PIN, true, true, true);

	/*Pin interrupt block*/
	/*Enable PININT clock */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PININT);

	/* Reset the PININT block */
	Chip_SYSCTL_PeriphReset(RESET_PININT);

	/* Configure interrupt channel for the GPIO pin in INMUX block */
	Chip_INMUX_PinIntSel(0, SW1_PORT, SW1_PIN);
	Chip_INMUX_PinIntSel(1, SW2_PORT, SW2_PIN);
	Chip_INMUX_PinIntSel(2, SW3_PORT, SW3_PIN);
	Chip_INMUX_PinIntSel(3, SW4_PORT, SW4_PIN);

	/* Configure channel interrupt as edge sensitive and rising edge interrupt */
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(0));
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(0));

	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(1));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(1));
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(1));

	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(2));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(2));
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(2));

	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(3));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(3));
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(3));

	/* Enable interrupt in the NVIC */
	NVIC_ClearPendingIRQ(PIN_INT0_IRQn);
	NVIC_EnableIRQ(PIN_INT0_IRQn);
	NVIC_ClearPendingIRQ(PIN_INT1_IRQn);
	NVIC_EnableIRQ(PIN_INT1_IRQn);
	NVIC_ClearPendingIRQ(PIN_INT2_IRQn);
	NVIC_EnableIRQ(PIN_INT2_IRQn);
	NVIC_ClearPendingIRQ(PIN_INT3_IRQn);
	NVIC_EnableIRQ(PIN_INT3_IRQn);
	/*End of pin interrupt setup block*/

	//LCD pins
	DigitalIoPin rs(0, 8, false, false);
	DigitalIoPin en(1, 6, false, false);
	DigitalIoPin d4(1, 8, false, false);
	DigitalIoPin d5(0, 5, false, false);
	DigitalIoPin d6(0, 6, false, false);
	DigitalIoPin d7(0, 7, false, false);

	Chip_RIT_Init(LPC_RITIMER);

	LiquidCrystal lcd(&rs, &en, &d4, &d5, &d6, &d7);
	// configure display geometry
	lcd.begin(16, 2);
	FanControl fctrl;
	SDP600 ps;
	menu = new Menu(lcd, fctrl, ps, dbgu);

	printf("Ready\r\n");

	while(1){

		while (!RingBuffer_IsEmpty(&rBuf)){
			Event ePtr;
			RingBuffer_Pop(&rBuf, &ePtr);
			menu->handleEvent(ePtr);
		}
		__WFI();
	}
	return 1;
}

