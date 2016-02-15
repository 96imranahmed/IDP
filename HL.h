#pragma once
#ifndef HL_H
#define HL_H

using namespace std;
#include <vector>
#include <robot_instr.h>
#include <robot_link.h>
class HardwareLayer;
#include "Sensor_ground.h"
#include "Navigation.h"
#include "Motor_ground.h"
#include "Actuator.h"
#include "Actions.h"
#include "LED.h"

#define ROBOT_NUM 11

//pick ripe first or pick overripe first?
//comment the following line to pick overripe first
#define RIPEFIRST

//MAIN INTERFACE

class HardwareLayer {
public:
	HardwareLayer();
	State cur_action;
    Sensor_Ground gnd_left, gnd_right, gnd_centre, rear_left;
	Motor_Ground gnd_drive;
	Actuator act_dumper, act_claw;
	Motor_Arm arm_motor;
    Navigation nav;
    Sensor_Collection sensorCol;
    LED LED_ripe, LED_over_ripe, LED_conveyor;
    LED LED_count0, LED_count1, LED_count2;
    LED_Collection LED_counters;
    Microswitch switch_high, switch_low, switch_conveyor;
    Actions actions;

    bool isError;
	robot_link rlink;
	int port2; //stores the value of port 2
	
	void execute();
};
#endif /*HL_H*/
