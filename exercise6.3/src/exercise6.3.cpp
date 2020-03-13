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
//#include "Sleeper.h"
#include "Blinker.h"
#include "ITMwrapper.h"
#include "DigitalIoPin.h"
#include "LiquidCrystal.h"
#include "BarGraph.h"
#include <cstdio>
#include "SimpleRingBuffer.h"

/* I2CM transfer record */
static I2CM_XFER_T  i2cmXferRec;
/* I2C clock is set to 1.8MHz */
#define I2C_CLK_DIVIDER         (40)
/* 100KHz I2C bit-rate */
#define I2C_BITRATE         (50000)
/* Standard I2C mode */
#define I2C_MODE    (0)
//upper and lower range of temperature readings
#define T_HIGH 125
#define T_LOW -40
#define BAR_L 8
#define BAR_N 16

#if defined(BOARD_NXP_LPCXPRESSO_1549)
/** 7-bit I2C addresses of Temperature Sensor */
#define I2C_TEMP_ADDR_7BIT  (0x48)
#endif
// TODO: insert other include files here

// TODO: insert other definitions and declarations here
static int ReadTC74Status(Sleeper &);
/* Initializes pin muxing for I2C interface - note that SystemInit() may
   already setup your pin muxing at system startup */
static void Init_I2C_PinMux(void)
{
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 22, IOCON_DIGMODE_EN | I2C_MODE);
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 23, IOCON_DIGMODE_EN | I2C_MODE);
	Chip_SWM_EnableFixedPin(SWM_FIXED_I2C0_SCL);
	Chip_SWM_EnableFixedPin(SWM_FIXED_I2C0_SDA);
}
/* Start ADC calibration */
void ADC_StartCalibration(LPC_ADC_T *pADC)
{
	// clock divider is the lowest 8 bits of the control register
	/* Setup ADC for about 500KHz (per UM) */
	uint32_t ctl = (Chip_Clock_GetSystemClockRate() / 500000) - 1;
	/* Set calibration mode */
	ctl |= ADC_CR_CALMODEBIT;
	pADC->CTRL = ctl;
	/* Calibration is only complete when ADC_CR_CALMODEBIT bit has cleared */
	while(pADC->CTRL & ADC_CR_CALMODEBIT) { };
}


/* Setup I2C handle and parameters */
static void setupI2CMaster()
{
	/* Enable I2C clock and reset I2C peripheral - the boot ROM does not
	   do this */
	Chip_I2C_Init(LPC_I2C0);

	/* Setup clock rate for I2C */
	Chip_I2C_SetClockDiv(LPC_I2C0, I2C_CLK_DIVIDER);

	/* Setup I2CM transfer rate */
	Chip_I2CM_SetBusSpeed(LPC_I2C0, I2C_BITRATE);

	/* Enable Master Mode */
	Chip_I2CM_Enable(LPC_I2C0);
}

static void SetupXferRecAndExecute(uint8_t devAddr,
		uint8_t *txBuffPtr,
		uint16_t txSize,
		uint8_t *rxBuffPtr,
		uint16_t rxSize)
{
	/* Setup I2C transfer record */
	i2cmXferRec.slaveAddr = devAddr;
	i2cmXferRec.status = 0;
	i2cmXferRec.txSz = txSize;
	i2cmXferRec.rxSz = rxSize;
	i2cmXferRec.txBuff = txBuffPtr;
	i2cmXferRec.rxBuff = rxBuffPtr;

	Chip_I2CM_XferBlocking(LPC_I2C0, &i2cmXferRec);
}

/* Master I2CM receive in polling mode */
#if defined(BOARD_NXP_LPCXPRESSO_1549)
/* Function to read LM75 I2C temperature sensor and output result */


static int ReadTC74Status(Sleeper &s){
	s.sleep(2);
	uint8_t ret = 0;
	uint8_t lm75TempRegisterAddress = 1;
	SetupXferRecAndExecute(I2C_TEMP_ADDR_7BIT, &lm75TempRegisterAddress, 1,
			&ret, 1);
	return (ret & 0x40);
}

static int8_t ReadTemperatureI2CM()
{
	uint8_t ret = 0;
	uint8_t lm75TempRegisterAddress = 0;
	ITM_wrapper iw;

	/* Read LM75 temperature sensor */
	SetupXferRecAndExecute(

			/* The LM75 I2C bus address */
			I2C_TEMP_ADDR_7BIT,

			/* Transmit one byte, the LM75 temp register address */
			&lm75TempRegisterAddress, 1,

			/* Receive back one byte, the contents of the temperature register */
			&ret, 1);

	/* Test for valid operation */
	if (i2cmXferRec.status != I2CM_STATUS_OK){
		ret = 0;
	}

	return (int8_t) ret;
}
#endif

int main(void) {
#if defined (__USE_LPCOPEN)
	// Read clock settings and update SystemCoreClock variable
	SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
	// Set up and initialize all required blocks and
	// functions related to the board hardware
	Board_Init();
#endif
#endif
	// TODO: insert code here
	SystemCoreClockUpdate();

	/* Setup I2C pin muxing */
	Init_I2C_PinMux();

	/* Allocate I2C handle, setup I2C rate, and initialize I2C
		   clocking */
	setupI2CMaster();

	/* Disable the interrupt for the I2C */
	NVIC_DisableIRQ(I2C0_IRQn);

	ITM_wrapper wr;

	Chip_RIT_Init(LPC_RITIMER);

	//LCD pins
	DigitalIoPin rs(0, 8, false, false);
	DigitalIoPin en(1, 6, false, false);
	DigitalIoPin d4(1, 8, false, false);
	DigitalIoPin d5(0, 5, false, false);
	DigitalIoPin d6(0, 6, false, false);
	DigitalIoPin d7(0, 7, false, false);

	LiquidCrystal lcd(&rs, &en, &d4, &d5, &d6, &d7);
	// configure display geometry
	lcd.begin(16, 2);
	BarGraph bg(&lcd, BAR_L, true);

	//LED
	Blinker s;
	SimpleRingBuffer rb(BAR_N, 0);

	int8_t temp;
	int records = 0;
	char str[80];
	char int_str[5];

	while(1) {
		//wait for sensor
		while (!ReadTC74Status(s)){}
		wr.print(str);
		temp = ReadTemperatureI2CM();
		sprintf(str, "Temperature read: %d\n", temp);
		wr.print(str);
		rb.push(temp);
		++records;

		//lcd section, upper row
		lcd.setCursor(0,0);
		//Oldest reading.  If less than BAR_N records made, we need to find the oldest manually
		sprintf(int_str, "%-3d", rb[(records < BAR_N) ? records - 1 : BAR_N - 1]);
		lcd.print(int_str);
		lcd.setCursor(13,0);
		sprintf(int_str, "%d", temp);
		lcd.print(int_str);

		//lcd lower row
		for (int i = 0; i < BAR_N; i++){
			//bars start from right
			lcd.setCursor(15 - i, 1);
			//each degree = 1 pixel
			bg.draw(rb[i] - 20);

		}
		s.sleep(5000);

	}
	return 0 ;
}
