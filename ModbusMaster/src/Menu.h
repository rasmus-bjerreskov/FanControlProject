/*
 * Menu.h
 *
 *  Created on: 9. mar. 2020
 *      Author: Rasmus
 */
#include <cstring>

#include "LiquidCrystal.h"
#include "Event.h"
#include "FanControl.h"
#include "SDP600.h"
#include "LpcUart.h"
extern volatile uint32_t systicks;

#ifndef MENU_H_
#define MENU_H_

class Menu {
public:
	Menu(LiquidCrystal& lcd_, FanControl& fctrl_, SDP600& ps_, LpcUart& dbg_);
	void handleEvent(const Event&);
	virtual ~Menu();

private:
	enum state{s_manSel, s_autSel, s_autModeRun, s_manModeRun,
		s_autModeAdjust, s_manModeAdjust, s_pFailure};
	//lcd information, in case we wanted to handle display in a separate class
	struct MENU_VALUES_T{
		char firstLine[17]; 	//lcd top row
		char secondLine[17]; 	//lcd bottom row
		uint8_t setSpeed; 		//currently configured speed
		uint8_t setPressure; 	//currently configured pressure
		uint8_t curConf; 		//tmp of the currently configured value
		uint8_t curSpeed; 		//current speed reading
		uint8_t curPressure; 	//current pressure reading

	};
	MENU_VALUES_T mv;
	LiquidCrystal& lcd;
	FanControl& fCtrl;
	SDP600& ps;
	LpcUart& dbg;
	state currentState;
	state lastState;

	char strbuf[17];
	uint32_t timeout;
	bool manualOverride;	//determines whether to run fan manually or automatically
	uint32_t failTimer; 	//for timekeeping of the current attempt

	uint16_t kp; 	//proportional term
	float ki;		//integral term
	float kd; 		//derivative term
	void setPIDVals();

	void setState(state newState);

	void manSel(const Event&); 			//manual mode highlighted
	void autSel(const Event&); 			//automatic mode highlighted
	void autModeRun(const Event&); 		//automatic mode, no unconfirmed value
	void manModeRun(const Event&); 		//manual mode, no unconfirmed value
	void autModeAdjust(const Event&); 	//automatic mode w. unconfirmed value
	void manModeAdjust(const Event&); 	//manual mode w. unconfirmed value
	void pFailure(const Event&); 		//failed to reach target pressure

	void setConfigString(uint8_t);
	void updateMenu();
	void tickUpdate();
	void updateStats();
	void refreshCtrl();

	void limitValue(uint8_t& val, uint8_t low, uint8_t high);
	void limitValue(int16_t& val, int16_t low, int16_t high);
};

#endif /* MENU_H_ */
