#include <robot_delay.h>
#include <iostream>
#include "HL.h"

#pragma once
#ifndef NAVIGATION_H
#define NAVIGATION_H

using namespace std;

extern HardwareLayer HL;

//line following and driving constants
//Constants
const int FORWARD_SPEED = 127;
const int TURNING_SPEED = 100;

//ENUMS
enum NavAction {
	TURN_RIGHT,
	TURN_LEFT,
	LEFT_VEER,
	RIGHT_VEER,
	LEFT_VEER_LARGE,
	RIGHT_VEER_LARGE,
	LEFT_VEER_SLIGHT,
	RIGHT_VEER_SLIGHT,
	STRAIGHT,
	TURN_AROUND_CW,
	TURN_AROUND_CCW,
	REVERSE
};
enum Direction {
	NORTH, //y
	EAST, //x
	SOUTH, //-y
	WEST, //-x
};
enum Node_Type {
	CROSSROAD, //-|-
	LINE_STRAIGHT
};

class Navigation {
public:
	Navigation();

	void performTurn(int exits, bool anti);
	void lossCorrection(bool veer_right);
	bool performSweep(int time, bool anti);
	void performLineCorrect(int speed_factor);
	void forwardLineCorrect(int speed_factor);
	
	//functions for fixed, predefined routes
	void startToRack(); ////S to (190,195)
	void retrieval(); //(190,195) to (190,120) //to be handled by Pickup
	void rackToD3(); //(190,195) to (150,220)
	void rackToConveyor(); //(190,195) to (40,120)
	void d3ToRack(); //(150,195) to (190,195)
	void d3ToStart();
	void conveyorToStart(); //(40,120) to S
	void conveyorToRack();
	
	Direction bearing;
	std::string bearingToString();
	NavAction cur_action;
	unsigned int junction;
	Node_Type Junction_Check();
private:
	int STANDARD_VEER_DEGREE_HIGH, STANDARD_VEER_DEGREE_LOW;
	int EXTREME_VEER_DEGREE_HIGH, EXTREME_VEER_DEGREE_LOW;
	
	void performMove(int speed_factor, int dest);
	bool handleJunction(unsigned int dest);
};
#endif /*NAVIGATION_H*/
