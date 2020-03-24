/*
 * Menu.cpp
 *
 *  Created on: 9. mar. 2020
 *      Author: Rasmus
 */

#include "Menu.h"

Menu::Menu(LiquidCrystal& lcd_, FanControl& fCtrl_, SDP600& ps_, LpcUart& dbg_) : //menu constructor
lcd(lcd_), fCtrl(fCtrl_), ps(ps_), dbg(dbg_) {
	mv.setSpeed = 0;
	mv.curConf = 0;

	setPIDVals();

	timeout = systicks;
	manualOverride = true;
	tickUpdate();
	setState(s_manSel);
}

void Menu::setState(state newState){
	lastState = currentState;
	currentState = newState;
	handleEvent(Event(Event::enter));
}

void Menu::handleEvent(const Event& e){
	if (e.type != Event::tick) timeout = systicks; //reset timeout on any user input

	switch(currentState){
	case s_autSel: autSel(e); break;
	case s_manSel: manSel(e); break;
	case s_autModeAdjust: autModeAdjust(e); break;
	case s_manModeAdjust: manModeAdjust(e); break;
	case s_autModeRun: autModeRun(e); break;
	case s_manModeRun: manModeRun(e); break;
	case s_pFailure: pFailure(e); break;
	default: break;
	}
}

/*Manual mode highlighted*/
void Menu::manSel(const Event& e){ // manual mode selection event
	switch(e.type){
	case Event::enter:
		strcpy(mv.firstLine,  ">Manual   ");
		strcpy(mv.secondLine, " Auto     ");
		updateMenu();
		break;

	case Event::up:
		setState(s_autSel);
		break;

	case Event::down:
		setState(s_autSel);
		break;

	case Event::ok:
		setState(s_manModeRun);
		break;

	case Event::tick:
		tickUpdate();
		break;

	default:
		break;
	}
}

/*Auto mode highlighted*/
void Menu::autSel(const Event& e){
	switch(e.type){
	case Event::enter:
		strcpy(mv.firstLine,  " Manual   ");
		strcpy(mv.secondLine, ">Auto     ");
		updateMenu();
		break;

	case Event::up:
		setState(s_manSel);
		break;

	case Event::down:
		setState(s_manSel);
		break;

	case Event::ok:
		setState(s_autModeRun);
		break;

	case Event::tick:
		tickUpdate();
		break;

	default:
		break;
	}
}

/*Show current configured pressure*/
void Menu::autModeRun(const Event& e){
	switch(e.type){
	case Event::enter:
		mv.curConf = mv.setPressure;
		strcpy(mv.firstLine, "Pressure  ");
		setConfigString(mv.setPressure);
		updateMenu();
		break;

	case Event::up:
		limitValue(++mv.curConf, 0, 120);
		setState(s_autModeAdjust);
		break;

	case Event::ok:
		manualOverride = false;
		failTimer = systicks;
		break;

	case Event::down:
		limitValue(--mv.curConf, 0, 120);
		setState(s_autModeAdjust);
		break;

	case Event::back:
		setState(s_autSel);
		break;

	case Event::tick:
		if (systicks - timeout >= 5000){
			handleEvent(Event(Event::back));
			timeout = systicks;
		}
		tickUpdate();
		break;

	default:
		break;
	}
}

/*Show current configured speed*/
void Menu::manModeRun(const Event& e){ // run manual mode
	switch(e.type){
	case Event::enter:
		mv.curConf = mv.setSpeed;
		strcpy(mv.firstLine, "Speed     ");
		setConfigString(mv.setSpeed);
		updateMenu();
		break;

	case Event::up:
		limitValue(++mv.curConf, 0, 100);
		setState(s_manModeAdjust);
		break;

	case Event::down:
		limitValue(--mv.curConf, 0, 100);
		setState(s_manModeAdjust);
		break;

	case Event::ok:
		manualOverride = true;
		fCtrl.speedPercent(mv.setSpeed);
		break;

	case Event::back:
		setState(s_manSel);
		break;

	case Event::tick:
		if (systicks - timeout >= 5000){
			handleEvent(Event(Event::back));
			timeout = systicks;
		}
		tickUpdate();
		break;

	default:
		break;
	}
}

/*Currently configuring pressure*/
void Menu::autModeAdjust(const Event& e){
	switch(e.type){
	case Event::enter:
		strcpy(mv.firstLine, "Pressure  ");
		setConfigString(mv.curConf);
		updateMenu();
		break;

	case Event::up:
		limitValue(++mv.curConf, 0, 120);
		setConfigString(mv.curConf);
		updateMenu();
		break;

	case Event::down:
		limitValue(--mv.curConf, 0, 120);
		setConfigString(mv.curConf);
		updateMenu();
		break;

	case Event::ok:
		mv.setPressure = mv.curConf;
		failTimer = systicks;		//we start a new attempt
		setState(s_autModeRun);
		manualOverride = false;
		break;

	case Event::back:
		setState(s_autModeRun);
		break;

	case Event::tick:
		tickUpdate();
		break;

	default:
		break;
	}
}

/*Currently configuring speed*/
void Menu::manModeAdjust(const Event& e){
	switch(e.type){
	case Event::enter:
		strcpy(mv.firstLine, "Speed     ");
		setConfigString(mv.curConf);
		updateMenu();
		break;

	case Event::up:
		limitValue(++mv.curConf, 0, 100);
		setConfigString(mv.curConf);
		updateMenu();
		break;

	case Event::down:
		limitValue(--mv.curConf, 0, 100);
		setConfigString(mv.curConf);
		updateMenu();
		break;

	case Event::ok:
		mv.setSpeed = mv.curConf;
		manualOverride = true;
		fCtrl.speedPercent(mv.setSpeed);
		setState(s_manModeRun);
		break;

	case Event::back:
		setState(s_manModeRun);
		break;

	case Event::tick:
		tickUpdate();
		break;

	default:
		break;
	}
}

/*Failed to reach target pressure within time limit*/
void Menu::pFailure(const Event& e){
	switch (e.type){
	case Event::enter:
		strcpy(mv.firstLine,  "Err: P NOT");
		strcpy(mv.secondLine, "POSSIBLE  ");
		updateMenu();
		break;

	case Event::ok:
		handleEvent(Event(Event::back));
		break;

	case Event::back:
		setState(lastState);
		failTimer = systicks;
		break;

	case Event::tick:
		tickUpdate();
		break;

	default:
		break;
	}

}

/*Sets strings of configured value in both configure states*/
void Menu::setConfigString(uint8_t val){
	strcpy(strbuf, "Error");
	if (currentState == s_autModeAdjust || currentState == s_manModeAdjust)
		sprintf(strbuf, " >%3d     ", val);
	else if (currentState == s_autModeRun || currentState == s_manModeRun)
		sprintf(strbuf, "  %3d     ", val);
	strcpy(mv.secondLine, strbuf);
}

/*Updates left side of display*/
void Menu::updateMenu(){
	lcd.setCursor(0, 0);
	lcd.print(mv.firstLine);
	lcd.setCursor(0, 1);
	lcd.print(mv.secondLine);
}

void Menu::tickUpdate(){
	updateStats();
	if (currentState != s_pFailure) //controller will be halted while error message is up
		refreshCtrl();
}

/*Update sensor readings*/
void Menu::updateStats(){
	static const uint32_t DELAY(200); //delay between readings
	static uint32_t time = 0;

	if (systicks - time >= DELAY){ //update sensor values
		mv.curPressure = ps.getPressure();
		mv.curSpeed = (float)fCtrl.getSpeed() / 20000 * 100;

		if(manualOverride)
			sprintf(strbuf, "*S %3d", mv.curSpeed);
		else
			sprintf(strbuf, " S %3d", mv.curSpeed);
		lcd.setCursor(10, 0);
		lcd.print(strbuf);
		if(!manualOverride)
			sprintf(strbuf, "*P %3d", mv.curPressure);
		else
			sprintf(strbuf, " P %3d", mv.curPressure);
		lcd.setCursor(10,1);
		lcd.print(strbuf);

		time = systicks;
	}
}

/*Automatic control*/
void Menu::refreshCtrl(){
	static const uint16_t MAX_CALIBRATION_TIME = 30000;
	static uint32_t timer;
	static uint8_t successCount = 0; //consecutive pressure readings within range
	static int16_t freq = 0;
	static int16_t err;
	static int16_t lastErr = 0;
	static int integral = 0;
	static int16_t deriv;

	if(!manualOverride && systicks - timer >= 2000){
		timer = systicks;

		//PID control
		err = mv.setPressure - mv.curPressure;
		integral += err;
		deriv = err - lastErr;
		lastErr = err;

		freq = (kp * err + ki * integral + kd * deriv) + 10000;
		limitValue(freq, 0, 20000);
		fCtrl.setFrequency(freq);

		//For debugging
		char stringbuf[80];
		sprintf(stringbuf, "%d %d %d\r\n", (int)kp*err, (int)ki*integral, (int)kd*deriv);
		dbg.write(stringbuf);

		//Failure timer will reset when two consecutive counts made
		if(mv.curPressure <= mv.setPressure + 2 && mv.curPressure >= mv.setPressure - 2)
			++successCount;
		else
			successCount = 0;
		if(successCount >= 2)
			failTimer = systicks;

		if (systicks - failTimer >= MAX_CALIBRATION_TIME){
			setState(s_pFailure);
		}
	}
}

/*Limits a value to within given paramenters*/
void Menu::limitValue(uint8_t& val, uint8_t low, uint8_t high){
	if(val < low)
		val = low;
	else if(val > high)
		val = high;
}

void Menu::limitValue(int16_t& val, int16_t low, int16_t high){
	if(val < low)
		val = low;
	else if(val > high)
		val = high;
}

void Menu::setPIDVals(){
	kp = 150; //207? //90
	ki = 8.5; //0.1
	kd = -61; // -70?
}

Menu::~Menu() {
}
