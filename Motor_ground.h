#include "HL.h"

#pragma once
#ifndef MOTOR_GROUND_H
#define MOTOR_GROUND_H

using namespace std;

extern HardwareLayer HL;

// Motor classes
class Motor_Ground {
public:
    int speed_left;
	int speed_right;
	Motor_Ground() {}; // unnecessary??
	void forward(int speed); //Pass - speed to go backwards
	void reverse(int speed);
	void differential_speed(int left, int right);
	void turn(int speed); //+ Gives turn in RH Direction
	void stop();
private:
	int normaliseSpeed(int speed_in);
	void setBothSpeed(int speed_in);
	void setDifferentialSpeed(int left, int right);
};
#endif /*MOTOR_GROUND_H*/
