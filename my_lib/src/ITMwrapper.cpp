/*
 * ITMwrapper.cpp
 *
 *  Created on: 24. jan. 2020
 *      Author: Rasmus
 */

#include "ITMwrapper.h"

ITM_wrapper::ITM_wrapper() {
	ITM_init();
}

void ITM_wrapper::print(const char* c){
	ITM_write(c);
}
void ITM_wrapper::print(const std::string& s){
	ITM_write(s.c_str());
}

void ITM_wrapper::print(int i){
	char str[11];
	snprintf(str, 10, "%d", i);
	ITM_write(str);
}

ITM_wrapper::~ITM_wrapper() {
	// TODO Auto-generated destructor stub
}

