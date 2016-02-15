using namespace std;

#include "Actuator.h"
#include <robot_delay.h>

//Actuator
Actuator::Actuator(Control_Type type) {
	cur_type = type;
	switch (type) {
	case ACTUATOR_CLAW:
		pos = 1<<6; 
		break;
	case ACTUATOR_DUMPER:
		pos = 1<<7;
		break;
	default:
		cout << "invalid actuator type" << endl;
		break;
	}
}

//public methods
void Actuator::extend() {
	if (cur_type == ACTUATOR_DUMPER) {
		change(false);
	} else {
		change(true);
	}
}

void Actuator::retract() {
	if (cur_type == ACTUATOR_DUMPER) {
		change(true);
	} else {
		change(false);
	}
}

std::string Actuator::toString() {
	switch(cur_val) {
	case EXTENDED:
		return "Extended";
		break;
	case RETRACTED:
		return "Retracted";
		break;
	default:
		return "invalid cur_val";
    }
}

//private methods
void Actuator::change(bool dir) {
	// direction: 1 for extending, 0 for retracting
	HL.port2 = HL.rlink.request(READ_PORT_2);
	if (dir) {
		HL.port2 |= pos;
		cur_val = EXTENDED;
	} else {
		HL.port2 &= ~pos;
		cur_val = RETRACTED;
	}
	HL.rlink.command(WRITE_PORT_2, HL.port2); 
}

//Motor_Arm
//public methods
void Motor_Arm::grab() {
	if  (position == TOP) {
		timedelay = DOWN_TOP2BOTTOM;
		move(BOTTOM, false);
	}
}

void Motor_Arm::dump() {
	if (position == BOTTOM) {
		timedelay = UP_BOTTOM2TOP;
		move(TOP, true);
	}
}

void Motor_Arm::stop() {
	HL.rlink.command(MOTOR_3_GO, 0);
}

std::string Motor_Arm::toString() {
	switch(position) {
	case TOP:
		return "Top";
		break;
	case BOTTOM:
		return "Bottom";
		break;
    }
    return "Null";
}

//private methods
void Motor_Arm::move(Motor_Position dest, bool dir) {
	//dir: true = upwards, false = downwards
	int speed = dir ? 127 : (127 + 128);
	HL.rlink.command(MOTOR_3_GO, speed);
	
	delay(timedelay); //MAX_SPEED
	HL.rlink.command(MOTOR_3_GO, 0);//stop
	position = dest;
}
