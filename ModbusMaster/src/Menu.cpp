/*
 * Menu.cpp
 *
 *  Created on: 9. mar. 2020
 *      Author: Rasmus
 */

#include "Menu.h"

#define PID_TUNING

Menu::Menu(LiquidCrystal& lcd_, FanControl& fCtrl_, SDP600& ps_, LpcUart& dbg_) : //menu constructor
lcd(lcd_), fCtrl(fCtrl_), ps(ps_), dbg(dbg_) {
#ifndef PID_TUNING
	mv.setSpeed = 0;
#else
	mv.setPressure = 70;
#endif
	mv.curConf = 0;

	setPIDVals();

	timeout = systicks;
	manualOverride = true;
	tickUpdate();
#ifndef PID_TUNING
	setState(s_manSel);
#else
	setState(s_autModeAdjust);
#endif
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
	default: break; //TODO error state?
	}
}

void Menu::manSel(const Event& e){ // manual mode selection event
	switch(e.type){
	case Event::enter:
		strcpy(mv.firstLine,  ">Manual   ");
		strcpy(mv.secondLine, " Automatic");
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

void Menu::autSel(const Event& e){ // automatic mode selection event
	switch(e.type){
	case Event::enter:
		strcpy(mv.firstLine,  " Manual   ");
		strcpy(mv.secondLine, ">Automatic");
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

void Menu::autModeRun(const Event& e){
	switch(e.type){
	case Event::enter:
		mv.curConf = mv.setPressure;
		strcpy(mv.firstLine, "Pressure  ");
		setConfigString(mv.setPressure);
		updateMenu();
		break;

	case Event::up:
		++mv.curConf;
		setState(s_autModeAdjust);
		break;

	case Event::ok:
		manualOverride = false;
		break;

	case Event::down:
		--mv.curConf;
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

void Menu::manModeRun(const Event& e){ // run manual mode
	switch(e.type){
	case Event::enter:
		mv.curConf = mv.setSpeed;
		strcpy(mv.firstLine, "Speed     ");
		setConfigString(mv.setSpeed);
		updateMenu();
		break;

	case Event::up:
		++mv.curConf;
		setState(s_manModeAdjust);
		break;

	case Event::down:
		--mv.curConf;
		setState(s_manModeAdjust);
		break;

	case Event::ok:
		manualOverride = true;
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

void Menu::autModeAdjust(const Event& e){
	switch(e.type){
	case Event::enter:
		manualOverride = false;
		strcpy(mv.firstLine, "Pressure  ");
		setConfigString(mv.curConf);
		updateMenu();
		break;

	case Event::up:
#ifndef PID_TUNING
		++mv.curConf;
		setConfigString(mv.curConf);
#else
		kd -= 0.5;
#endif
		updateMenu();
		break;

	case Event::down:
#ifndef PID_TUNING
		--mv.curConf;
		setConfigString(mv.curConf);
#else
		kp += 1;
#endif
		updateMenu();
		break;

	case Event::ok:
		manualOverride = false;
#ifndef PID_TUNING
		mv.setPressure = mv.curConf;
		failTimer = systicks;		//we start a new attempt
		setState(s_autModeRun);
#else
		ki += 0.05;
#endif
		break;

	case Event::back:
#ifndef PID_TUNING
		setState(s_autModeRun);
#else
		setPIDVals();
#endif
		break;

	case Event::tick:
		tickUpdate();
		break;

	default:
		break;
	}
}
void Menu::manModeAdjust(const Event& e){
	switch(e.type){
	case Event::enter:
		strcpy(mv.firstLine, "Speed     ");
		setConfigString(mv.curConf);
		updateMenu();
		break;

	case Event::up:
		++mv.curConf;
		setConfigString(mv.curConf);
		updateMenu();
		break;

	case Event::down:
		--mv.curConf;
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

void Menu::pFailure(const Event& e){ // pressure not reached event
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
		failTimer = systicks; //TODO maybe change this? currently goes into a new attempt
		break;

	case Event::tick:
		tickUpdate();
		break;

	default:
		break;
	}

}

//Sets strings of configured value in both configure states
void Menu::setConfigString(uint8_t val){
	strcpy(strbuf, "Error");
	if (currentState == s_autModeAdjust || currentState == s_manModeAdjust)
		sprintf(strbuf, " >%3d     ", val);
	else if (currentState == s_autModeRun || s_manModeRun)
		sprintf(strbuf, "  %3d     ", val);
	strcpy(mv.secondLine, strbuf);
}

//Updates left side of display
void Menu::updateMenu(){
	lcd.setCursor(0, 0);
	lcd.print(mv.firstLine);
	lcd.setCursor(0, 1);
	lcd.print(mv.secondLine);
}

void Menu::tickUpdate(){
#ifdef PID_TUNING
	static uint32_t timer;
	if (systicks - timer >= 500){
		char stringbuf[50];
		sprintf(stringbuf, "kp: %d ki: %.3f kd: %.3f\r\n", kp, ki, kd);
		dbg.write(stringbuf);
		timer = systicks;
	}
	failTimer = systicks;
#endif
	updateStats();
	if (currentState != s_pFailure) //controller will be halted while error message is up
		refreshCtrl();
}

//Update sensor readings
void Menu::updateStats(){
	static const uint32_t DELAY(200); //delay between readings
	static uint32_t time = 0;

	if (systicks - time >= DELAY){ //update sensor values
		mv.curPressure = ps.getPressure();
		mv.curSpeed = (float)fCtrl.getSpeed() / 20000 * 100;

		sprintf(strbuf, "S %3d", mv.curSpeed);
		lcd.setCursor(11, 0);
		lcd.print(strbuf);
		lcd.setCursor(11,1);
		sprintf(strbuf, "P %3d", mv.curPressure);
		lcd.print(strbuf);

		if (mv.curSpeed == 71){
			printf("Wat."); //we kept seeing speed randomly displaying 71
		}
		time = systicks;
	}
}

//automatic control
void Menu::refreshCtrl(){
	static uint32_t timer;
	static uint8_t successCount = 0; 	/*Failure timer will reset when four successive counts made.
										  Timer should reset when vent is adjusted*/
	static int16_t freq = 0;
	static int16_t err;
	static int16_t lastErr = 0;
	static int integral = 0;
	static int16_t deriv;

	if(!manualOverride && systicks - timer >= 2000){
		timer = systicks;

		err = mv.setPressure - mv.curPressure;
		integral += err;
		deriv = err - lastErr;
		lastErr = err;

		freq += kp * err + ki * integral + kd * deriv;
		limitValue(&freq, 0, 20000);
		fCtrl.setFrequency(freq);
		char stringbuf[80];
		sprintf(stringbuf, "%d %d %d\r\n", (int)kp*err, (int)ki*integral, (int)kd*deriv);
		dbg.write(stringbuf);
		if(mv.curPressure <= mv.setPressure + 2 && mv.curPressure >= mv.setPressure - 2)
			++successCount;
		else
			successCount = 0;
		if(successCount >= 4)
			failTimer = systicks;
		if (systicks - failTimer >= 30000){
			setState(s_pFailure);
		}
	}
}

void Menu::limitValue(uint8_t* valptr, uint8_t low, uint8_t high){
	if(*valptr < low)
		*valptr = low;
	else if(*valptr > high)
		*valptr = high;
}

void Menu::limitValue(int16_t* valptr, int16_t low, int16_t high){
	if(*valptr < low)
		*valptr = low;
	else if(*valptr > high)
		*valptr = high;
}

void Menu::setPIDVals(){
	kp = 207; //207? //90
	ki = 0.0; //0.1
	kd = -0.005; // -70?
}

Menu::~Menu() {
}
