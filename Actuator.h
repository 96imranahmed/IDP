//contains actuator control and 3rd motor control.

using namespace std;
#include "HL.h"

#pragma once
#ifndef ACTUATOR_H
#define ACTUATOR_H

//External variables
extern HardwareLayer HL;

enum Actuator_Val {
	EXTENDED,
	RETRACTED
};
enum Control_Type {
	ACTUATOR_DUMPER,
	ACTUATOR_CLAW,
	ARM_MOTOR
};

enum Motor_Position {
	TOP,
	BOTTOM
};

class Actuator {
public:
    Control_Type cur_type;
    Actuator_Val cur_val;  
    Actuator(Control_Type type);
    Actuator() {};
    void extend();
    void retract();   
    std::string toString();
private:
	int val; //stores port value
    int pos; //bit number on I/O port
    void change(bool dir);
};

class Motor_Arm {
public: 
    Motor_Position position;
    Motor_Arm() {
		position = TOP;
		DOWN_TOP2BOTTOM = 8200;
		UP_BOTTOM2TOP = 13000;
	};
    void grab();
	void dump();
	void stop();
    std::string toString();   
private: 
    void move(Motor_Position dest, bool dir);

	int timedelay;
	int DOWN_TOP2BOTTOM, DOWN_TOP2MIDDLE, DOWN_MIDDLE2BOTTOM;
	int UP_BOTTOM2TOP, UP_BOTTOM2MIDDLE, UP_MIDDLE2TOP;
};

#endif /*ACTUATOR_H*/
