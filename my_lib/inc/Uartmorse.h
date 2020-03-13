/*
 * Uartmose.h
 *
 *  Created on: 31. jan. 2020
 *      Author: Rasmus
 */

#ifndef UARTMORSE_H_
#define UARTMORSE_H_
#include "LpcUart.h"
#include "morseencoder.h"
#include <cstring>

class Uart_morse {
public:
	Uart_morse(morse_encoder&, LpcUart&);
	virtual ~Uart_morse();
	void read_uart();
	char* gets();

private:
	morse_encoder& mo;
	LpcUart& uartr;
	static const int BUFLEN = 81;
	char sa[BUFLEN];
	char *s;
	int wpm;
};

#endif /* UARTMORSE_H_ */
