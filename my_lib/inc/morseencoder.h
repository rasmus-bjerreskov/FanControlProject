/*
 * morseencoder.h
 *
 *  Created on: 28. jan. 2020
 *      Author: Rasmus
 */

#ifndef MORSEENCODER_H_
#define MORSEENCODER_H_
#include <cctype>
#include <string>
#include "DigitalIoPin.h"
#include "Sleeper.h"

class morse_encoder {
public:
	morse_encoder(DigitalIoPin&);
	morse_encoder(DigitalIoPin&, DigitalIoPin&);
	void set_period(int);
	void set_wpm(int);
	int get_period();
	int get_wpm();
	void send(const char*);
	void send(std::string);
	virtual ~morse_encoder();

private:
	void morse_char(char);
	void put_signal(int);
	static const int DOT = 1;
	static const int DASH = 3;
	int time_unit;
	int wpm;
	DigitalIoPin &led;
	DigitalIoPin &decoder;
	Sleeper s;
	struct MorseCode {
		char symbol;
		unsigned char code[7];
	};
	static const MorseCode ITU_morse[];
};

#endif /* MORSEENCODER_H_ */
