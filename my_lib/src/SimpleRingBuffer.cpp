/*
 * SimpleRingBuffer.cpp
 *
 *  Created on: 20. feb. 2020
 *      Author: Rasmus
 */

#include "SimpleRingBuffer.h"

SimpleRingBuffer::SimpleRingBuffer(int length0, int init) {
	length = (length0 > ASIZE) ? ASIZE : length0;
	index = 0;
	old = 0;
	for(int i = 0; i < length0; i++){
		array[i] = init;
	}
	aptr = array;
}

/*
 * index moves back as items are added
 * eg [0, 0, 0] -> [0, 0, 1] -> [0, 2, 1] -> [3, 2, 1] -> [3, 2, 4]
 */
void SimpleRingBuffer::push(int nr){
	index  = old;
	aptr[index] = nr;
	--old;
	old = (old + length) % length;
}

//read-only
int& SimpleRingBuffer::operator[](int i) const{
	return aptr[(index + i) % length];
}

int* SimpleRingBuffer::end(){
	int* ret = &aptr[old];
	return ret;
}


SimpleRingBuffer::~SimpleRingBuffer() {
	// TODO Auto-generated destructor stub
}

