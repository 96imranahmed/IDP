#include "Motor_ground.h"

//Motor Methods
void Motor_Ground::forward(int speed) {
	if (normaliseSpeed(speed) != speed_left) {
		setBothSpeed(speed);
		HL.rlink.command(MOTOR_1_GO, speed_left);
		HL.rlink.command(MOTOR_2_GO, speed_right);
	}
}
void Motor_Ground::reverse(int speed) {
	if (normaliseSpeed(-1*speed) != normaliseSpeed(speed_right)) {
		setBothSpeed(-speed);
		HL.rlink.command(MOTOR_1_GO, speed_left);
		HL.rlink.command(MOTOR_2_GO, speed_right);
	}
}
void Motor_Ground::differential_speed(int left, int right) {
	if (normaliseSpeed(right) != speed_right || normaliseSpeed(left) != speed_left) {
		setDifferentialSpeed(left, right);
		HL.rlink.command(MOTOR_1_GO, speed_left);
		HL.rlink.command(MOTOR_2_GO, speed_right);
	}
}
void Motor_Ground::turn(int speed) {
	setBothSpeed(speed);
	HL.rlink.command(BOTH_MOTORS_GO_OPPOSITE, 0);
	delay(100); //Temporary Pause to prevent motor "Skidding"
	HL.rlink.command(BOTH_MOTORS_GO_SAME, speed_left); //CW turn (for + speed)
}

void Motor_Ground::stop(){
	this->forward(0);
}

int Motor_Ground::normaliseSpeed(int speed_in) {
    if (speed_in < 0) {
        if (speed_in < -127) speed_in = -127;
        speed_in = -1*speed_in + 128;
    } else {
        if (speed_in > 127) speed_in = 127;
    }
    return speed_in;
}

void Motor_Ground::setBothSpeed(int speed_in) {
    speed_left = normaliseSpeed(-1*speed_in);
    speed_right = normaliseSpeed(speed_in);
}

void Motor_Ground::setDifferentialSpeed(int left, int right) {
    speed_left = normaliseSpeed(-1*left);
    speed_right = normaliseSpeed(right);
}
