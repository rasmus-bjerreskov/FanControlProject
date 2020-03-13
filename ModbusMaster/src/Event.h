/*
 * Event.h
 *
 *  Created on: 9. mar. 2020
 *      Author: Rasmus
 */

#include <cstdint>

#ifndef EVENT_H_
#define EVENT_H_

class Event {
public:
	enum eventType{enter, up, down, ok, back, tick};
	Event(eventType e = tick);
	eventType type;
	virtual ~Event();
private:


};

#endif /* EVENT_H_ */
