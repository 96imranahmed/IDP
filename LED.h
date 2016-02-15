 #include "HL.h"
#include <robot_delay.h>

#pragma once
#ifndef LED_H
#define LED_H

using namespace std;

extern HardwareLayer HL;

enum LED_Type {
	CONVEYOR_ENABLE,
	OVER_RIPE_INDICATOR,
	RIPE_INDICATOR,
	COUNT0,
	COUNT1,
	COUNT2
};

class LED {
	LED_Type type;
	int pos;
	public:
	LED(LED_Type type_in);
	LED(){};
	void setVal(int on, bool repeat);
};

class LED_Collection {
public:
	LED_Collection(LED l0_in, LED l1_in, LED l2_in);
	LED_Collection(){};
	void setCount(int count);
	LED l0;
	LED l1;
	LED l2;
};

enum Microswitch_Type {
	OVER_RIPE_DETECT,
	RIPE_DETECT,
	CONVEYOR
};

class Microswitch {
public:
	Microswitch(){};
	Microswitch(Microswitch_Type type_in);
	bool hasUpdatedHigh();
	int getVal();
private:
	Microswitch_Type type;
	int port;

};
#endif /*LED_H*/
