#include "LED.h"
#include <robot_delay.h>
using namespace std;

LED::LED(LED_Type type_in) {
	type = type_in;
	switch (type) {
	case CONVEYOR_ENABLE: // NOT CURRENTLY USED
		pos = 1<<0; 
		break;
	case OVER_RIPE_INDICATOR: 
		pos = 1<<1;
		break;
	case RIPE_INDICATOR: 
		pos = 1<<2;
		break;
	case COUNT0:
		pos = 1<<3;
		break;
	case COUNT1:
		pos = 1<<4;
		break;
	case COUNT2:
		pos = 1<<5;
		break;
	}
}

void LED::setVal(int on, bool repeat) {
	if (!repeat) HL.port2 = HL.rlink.request(READ_PORT_2);
	if (on == 1) {	// turn on
		HL.port2 |= pos;
	} else if (on == 0) { //turn off
		HL.port2 &= ~pos;	
	}
	HL.rlink.command(WRITE_PORT_2, HL.port2);
}

LED_Collection::LED_Collection(LED l0_in, LED l1_in, LED l2_in) {
	l0 = l0_in;
	l1 = l1_in;
	l2 = l2_in;
}

void LED_Collection::setCount(int count) {
	if (count > 7) {count = 7;}
	if ((count % 2) == 1) { HL.LED_counters.l0.setVal(0, false);} else { HL.LED_counters.l0.setVal(1, false);}
	if ((count % 4) > 1 ) { HL.LED_counters.l1.setVal(0, false);} else { HL.LED_counters.l1.setVal(1, false);}
	if (count > 3) { HL.LED_counters.l2.setVal(0, false);} else { HL.LED_counters.l2.setVal(1, false);}
}

//Microswitch
Microswitch::Microswitch(Microswitch_Type type_in) {
	type = type_in;
	switch(type) {
	case OVER_RIPE_DETECT:
		port = 4;
		break;
	case RIPE_DETECT:
		port = 6;
		break;
	case CONVEYOR:
		port = 7;
		break;
	}
}

bool Microswitch::hasUpdatedHigh() {
	int val = getVal();
	if (val == 1) {
		int new_val = getVal();
		if (new_val == 1) return true;
	} 
	return false; 
}

int Microswitch::getVal() {
	int val = HL.rlink.request(READ_PORT_0);
	return (val & (1<<port))/(1<<port);
}
