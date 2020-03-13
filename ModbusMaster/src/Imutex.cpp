// implementation of interrupt locking “mutex”
#include "chip.h"
#include "Imutex.h"
Imutex::Imutex() : enable(false) {
}
Imutex::~Imutex(){

}
void Imutex::lock(){
	enable = (__get_PRIMASK() & 1) == 0;
	__disable_irq();
}
void Imutex::unlock(){
	if(enable) __enable_irq();
}
