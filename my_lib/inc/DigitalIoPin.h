/*
 * DigitalIoPin.h
 *
 *  Created on: 17. jan. 2020
 *      Author: Rasmus
 */

#ifndef DIGITALIOPIN_H_
#define DIGITALIOPIN_H_

class DigitalIoPin {
public:
	DigitalIoPin(int port0, int pin0, bool input0 = true, bool invert0 = false, bool pullup0 = true);
	virtual ~DigitalIoPin() {};
	bool read();
	//bool raw_read();
	void write(bool value);
private:
	int port;
	int pin;
	bool invert;
};
#endif /* DIGITALIOPIN_H_ */
