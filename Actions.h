//contains action determination logic

using namespace std;
#include "HL.h"
#include "LED.h"
#include <stopwatch.h>

#pragma once
#ifndef ACTIONS_H
#define ACTIONS_H

extern HardwareLayer HL;

enum State {
	NAVIGATING_TO_RACK,
	DUMPING_CONVEYOR,
	DUMPING_D3,
	PICKING_UP_RIPE,
	PICKING_UP_OVERRIPE,
	NAVIGATING_HOME
};

class Pickup {
public:
	Pickup(){};
	void execute();
	stopwatch watch;
private:
	void initialise();

	unsigned int LED_time;
	void realign();
	LED indicator;
	bool midpoint;
	void move(bool ignoreJunction);
	bool grab();
};

class Dump {
public:
	Dump(){};
	void execute();
private:
	stopwatch watch;
	void initialise();
	void performDump();
};

class Actions {
public:
Pickup pickup;
Dump dump;
Actions() {};
int num_taken;
};
#endif /*ACTIONS_H*/
