/*
 * DigitalIoPin.cpp
 *
 *  Created on: 17. jan. 2020
 *      Author: Rasmus
 */
#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>
#include "DigitalIoPin.h"

DigitalIoPin::DigitalIoPin(int port0, int pin0, bool input0, bool invert0, bool pullup0) : port(port0), pin(pin0), invert(invert0){

	if(input0) {
		Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin,
				((pullup0 ? IOCON_MODE_PULLUP : IOCON_MODE_PULLDOWN) | IOCON_DIGMODE_EN | (invert0 ? IOCON_INV_EN : 0)));
		Chip_GPIO_SetPinDIRInput(LPC_GPIO, port, pin);
	}
	else {
		Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin,
				(IOCON_MODE_INACT | IOCON_DIGMODE_EN ));
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, port, pin);
	}
}

bool DigitalIoPin::read() {
	return Chip_GPIO_GetPinState(LPC_GPIO, port,pin);
}
void DigitalIoPin::write(bool value) {
	if (invert)
		Chip_GPIO_SetPinState(LPC_GPIO, port, pin, !value);
	else Chip_GPIO_SetPinState(LPC_GPIO, port, pin, value);
}

