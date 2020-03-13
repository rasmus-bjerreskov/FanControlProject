/*
 * ITMwrapper.h
 *
 *  Created on: 24. jan. 2020
 *      Author: Rasmus
 */
#include "ITM_write.h"
#include <string>
#include <cstdio>
#ifndef ITMWRAPPER_H_
#define ITMWRAPPER_H_

class ITM_wrapper {
public:
	ITM_wrapper();
	void print(const char* c);
	void print(const std::string& s);
	void print(int i);
	void print(float f);
	virtual ~ITM_wrapper();
};

#endif /* ITMWRAPPER_H_ */
