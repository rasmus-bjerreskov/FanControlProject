/*
 * SimpleRingBuffer.h
 *
 *  Created on: 20. feb. 2020
 *      Author: Rasmus
 */
#ifndef SIMPLERINGBUFFER_H_
#define SIMPLERINGBUFFER_H_

class SimpleRingBuffer {
public:
	SimpleRingBuffer(int length, int initialvals);
	void push(int);
	int& operator[](int) const;
	int* end();
	int* begin();
	virtual ~SimpleRingBuffer();

private:
	int length;
	int index;
	int old;
	static const int ASIZE = 100;
	int array[ASIZE];
	int* aptr;


};

#endif /* SIMPLERINGBUFFER_H_ */
