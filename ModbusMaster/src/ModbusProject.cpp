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
#define PIN_INTERRUPT
//#define BOARD_DEBUG
#ifdef BOARD_DEBUG
#define SW2_PORT 0
#define SW2_PIN 17
#define SW3_PORT 1
#define SW3_PIN 11
#define SW4_PORT 1
#define SW4_PIN 9
#endif
#ifndef BOARD_DEBUG
#define SW1_PORT 0 	//"BACK"
#define SW1_PIN 10 	//"BACK"
#define SW2_PORT 0 	//"UP"
#define SW2_PIN 16 	//"UP"
#define SW3_PORT 1 	//"OK"
#define SW3_PIN 3  	//"OK"
#define SW4_PORT 0 	//"DOWN"
#define SW4_PIN 0  	//"DOWN
#endif

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

#ifdef PIN_INTERRUPT

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
#endif

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

#if 0

	Board_LED_Set(0, false);
	Board_LED_Set(1, true);
	printf("Started\n"); // goes to ITM console if retarget_itm.c is included



	const int n = 50;
	uint16_t f_vals[n];
	uint8_t p_vals[n];
	uint32_t delay = 1000;


	uint16_t freq = 0;
	uint8_t cur_p;
	int16_t err;
	int16_t lastErr = 0;
	int integral = 0;
	int16_t deriv;

	uint16_t kp = 400; //proportional term 400
	float ki = 0;//3; //integral term
	uint8_t kd = 500;//20; //derivative term

	uint32_t timer;
	uint32_t starttime = systicks;
	uint8_t target = 50;

	fctrl.speedPercent(0);
	sleep(500);
	fctrl.setFrequency(20000);
	timer = systicks;
	/*PID controller*/
	int i = 0;
	char strbuf[80];
	while(systicks - starttime < n * delay){
		if (systicks - timer > delay){
			timer = systicks;
			cur_p = pr.getPressure();

			err = target - cur_p;
			integral += err;
			deriv = err - lastErr;
			lastErr = err;

			freq += kp * err + ki * integral + kd * deriv;
			fctrl.setFrequency(freq);

			p_vals[i] = cur_p;
			f_vals[i] = freq;
			i++;
			sprintf(strbuf, "%d %d %d\r\n", i, cur_p, freq);
			dbgu.write(strbuf);

		}
	}

	fctrl.setFrequency(0);


	for (int i = 0; i < n; i++){
		sprintf(strbuf,"%d:: p: %d, f: %d, t: %lu\r\n", i+1, p_vals[i], f_vals[i], i*delay);
		dbgu.write(strbuf);
	}

	//	for (int i = 0; i <= 100; i+=5){
	//		fctrl.speedPercent(i);
	//		sleep(500);
	//	}

	fctrl.speedPercent(0);
#endif

	RingBuffer_Init(&rBuf, events, sizeof(Event), 20);

	DigitalIoPin SW1(SW1_PORT, SW1_PIN, true, true, true);
	DigitalIoPin SW2(SW2_PORT, SW2_PIN, true, true, true);
	DigitalIoPin SW3(SW3_PORT, SW3_PIN, true, true, true);
	DigitalIoPin SW4(SW4_PORT, SW4_PIN, true, true, true);

#ifdef PIN_INTERRUPT

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
#endif

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

#if 0
	menu->handleState(Event(Event::up));
	menu->handleState(Event(Event::ok));
	menu->handleState(Event(Event::up));
	menu->handleState(Event(Event::up));
	menu->handleState(Event(Event::up));
	menu->handleState(Event(Event::back));
	menu->handleState(Event(Event::up));
	menu->handleState(Event(Event::up));
	menu->handleState(Event(Event::up));
	menu->handleState(Event(Event::ok));
	menu->handleState(Event(Event::back));
#endif
	printf("Ready\r\n");

	while(1){
#ifndef PIN_INTERRUPT
		if(sw1.read()){
			menu->handleState(Event(Event::up));
			while (sw1.read()){
				sleep(10);
			}
		}
		if(sw2.read()){
			menu->handleState(Event(Event::ok));
			while (sw2.read()){
				sleep(10);
			}
		}
		if(sw3.read()){
			menu->handleState(Event(Event::down));
			while (sw3.read()){
				sleep(10);
			}
		}
#endif
		while (!RingBuffer_IsEmpty(&rBuf)){
			Event ePtr;
			RingBuffer_Pop(&rBuf, &ePtr);
			menu->handleEvent(ePtr);
		}
		__WFI();
	}
	return 1;
}

