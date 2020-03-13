/*
 * morseencoder.cpp
 *
 *  Created on: 28. jan. 2020
 *      Author: Rasmus
 */

#include "morseencoder.h"

const morse_encoder::MorseCode morse_encoder::ITU_morse[] = {
		{ 'A', { DOT, DASH } }, // A
		{ 'B', { DASH, DOT, DOT, DOT } }, // B
		{ 'C', { DASH, DOT, DASH, DOT } }, // C
		{ 'D', { DASH, DOT, DOT } }, // D
		{ 'E', { DOT } }, // E
		{ 'F', { DOT, DOT, DASH, DOT } }, // F
		{ 'G', { DASH, DASH, DOT } }, // G
		{ 'H', { DOT, DOT, DOT, DOT } }, // H
		{ 'I', { DOT, DOT } }, // I
		{ 'J', { DOT, DASH, DASH, DASH } }, // J
		{ 'K', { DASH, DOT, DASH } }, // K
		{ 'L', { DOT, DASH, DOT, DOT } }, // L
		{ 'M', { DASH, DASH } }, // M
		{ 'N', { DASH, DOT } }, // N
		{ 'O', { DASH, DASH, DASH } }, // O
		{ 'P', { DOT, DASH, DASH, DOT } }, // P
		{ 'Q', { DASH, DASH, DOT, DASH } }, // Q
		{ 'R', { DOT, DASH, DOT } }, // R
		{ 'S', { DOT, DOT, DOT } }, // S
		{ 'T', { DASH } }, // T
		{ 'U', { DOT, DOT, DASH } }, // U
		{ 'V', { DOT, DOT, DOT, DASH } }, // V
		{ 'W', { DOT, DASH, DASH } }, // W
		{ 'X', { DASH, DOT, DOT, DASH } }, // X
		{ 'Y', { DASH, DOT, DASH, DASH } }, // Y
		{ 'Z', { DASH, DASH, DOT, DOT } }, // Z
		{ '1', { DOT, DASH, DASH, DASH, DASH } }, // 1
		{ '2', { DOT, DOT, DASH, DASH, DASH } }, // 2
		{ '3', { DOT, DOT, DOT, DASH, DASH } }, // 3
		{ '4', { DOT, DOT, DOT, DOT, DASH } }, // 4
		{ '5', { DOT, DOT, DOT, DOT, DOT } }, // 5
		{ '6', { DASH, DOT, DOT, DOT, DOT } }, // 6
		{ '7', { DASH, DASH, DOT, DOT, DOT } }, // 7
		{ '8', { DASH, DASH, DASH, DOT, DOT } }, // 8
		{ '9', { DASH, DASH, DASH, DASH, DOT } }, // 9
		{ '0', { DASH, DASH, DASH, DASH, DASH } }, // 0
		{ 0, { 0 } } // terminating entry - Do not remove!
};

morse_encoder::morse_encoder(DigitalIoPin& led0) : led(led0), decoder(led0){
	led.write(false);
	time_unit = 100;
	wpm = 1000 / time_unit;
}

morse_encoder::morse_encoder(DigitalIoPin& led0, DigitalIoPin& decoder0) :
		led(led0), decoder(decoder0) {
	led.write(false);
	time_unit = 100;
	wpm = 1000 / time_unit;
}

void morse_encoder::set_period(int i){
	time_unit = i;
	wpm = 1000 / time_unit;
}

void morse_encoder::set_wpm(int i){
	wpm = i;
	time_unit = 1000 / wpm;
}

int morse_encoder::get_wpm(){
	return wpm;
}

int morse_encoder::get_period(){
	return time_unit;
}

void morse_encoder::send(const char* str){
	for(int i = 0; str[i] != '\0'; i++){
		morse_char(str[i]);
		s.sleep(2*time_unit);
	}
}

void morse_encoder::send(std::string s){
	send(s.c_str());
}

//fetch morse pattern of char
void morse_encoder::morse_char(char c){
	if (c == ' ') s.sleep(2*time_unit); //3 before, 2, 2 after
	else {
		c = toupper(c);
		int k = 0;

		while (ITU_morse[k].symbol != c && ITU_morse[k].symbol != 0){
			k++;
		}
		if (ITU_morse[k].symbol == 0) k = 23; //X if unknown

		for(int i = 0; ITU_morse[k].code[i] != 0; i++){
			put_signal(ITU_morse[k].code[i]);
		}
	}
}

//output morse unit
void morse_encoder::put_signal(int n){
	led.write(true);
	decoder.write(false);
	s.sleep(n*time_unit);
	decoder.write(true);
	led.write(false);
	s.sleep(time_unit); //inter-signal gap
}

morse_encoder::~morse_encoder() {
	// TODO Auto-generated destructor stub
}

