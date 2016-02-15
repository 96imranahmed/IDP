using namespace std;
#include "Navigation.h"
#include <stopwatch.h>
//NAVIGATION
//constructor
stopwatch watch;
Navigation::Navigation() {
	bearing = NORTH;
	cur_action = STRAIGHT;
	STANDARD_VEER_DEGREE_HIGH = (int) (10/127.0 * FORWARD_SPEED);
	STANDARD_VEER_DEGREE_LOW = (int) (-80/127.0 * FORWARD_SPEED);
	EXTREME_VEER_DEGREE_HIGH  = (int) (20/127.0 * FORWARD_SPEED);
	EXTREME_VEER_DEGREE_LOW  = (int) (-100/127.0 * FORWARD_SPEED);
};
//Check for junctions and following line
Node_Type Navigation::Junction_Check() {
	HL.sensorCol.updateAll();
	if ((HL.sensorCol.s1.check(LINE)) && (HL.sensorCol.s2.check(LINE)) && (HL.sensorCol.s3.check(LINE))) { 
		//cout<<"Crossroad"<<endl;
		return CROSSROAD;
	}
    if ((HL.sensorCol.s1.check(NO_LINE)) && (HL.sensorCol.s2.check(LINE)) && (HL.sensorCol.s3.check(NO_LINE))) {
		cur_action = STRAIGHT;
	}
    //Case Current Left Veer
    else if ((HL.sensorCol.s1.check(NO_LINE)) && (HL.sensorCol.s2.check(LINE)) && (HL.sensorCol.s3.check(LINE))) {
        //cout<<"Curr veer left"<<endl;
        cur_action = RIGHT_VEER;
    }
    //Case Current Right Veer
    else if ((HL.sensorCol.s1.check(LINE)) && (HL.sensorCol.s2.check(LINE)) && (HL.sensorCol.s3.check(NO_LINE))) {
        //cout<<"Curr veer right"<<endl;
        cur_action = LEFT_VEER;
    } 
    //Case Extreme Veer
    else if ((HL.sensorCol.s1.check(NO_LINE)) && (HL.sensorCol.s2.check(NO_LINE)) && (HL.sensorCol.s3.check(LINE))) {
		//cout<<"Curr veer left ext"<<endl;
        cur_action = RIGHT_VEER_LARGE;
    } 
    else if ((HL.sensorCol.s1.check(LINE)) && (HL.sensorCol.s2.check(NO_LINE)) && (HL.sensorCol.s3.check(NO_LINE))) {
        //cout<<"Curr veer right ext"<<endl;
        cur_action = LEFT_VEER_LARGE;
    }
    else if ((HL.sensorCol.s1.check(NO_LINE)) && (HL.sensorCol.s2.check(NO_LINE)) && (HL.sensorCol.s3.check(NO_LINE))) {
		if (cur_action == LEFT_VEER_LARGE || cur_action == LEFT_VEER) {lossCorrection(false);}
		else if (cur_action == RIGHT_VEER_LARGE || cur_action == RIGHT_VEER) {lossCorrection(true);}
		//cout << "LOST - Attempting Correction" << endl;
	}
    return LINE_STRAIGHT;
}

void Navigation::performTurn(int exits, bool anti) {
	//1 means L/R turn (depending on anti), 2 means Straight, 3 means R/L (depending on anti), 4 means full twirl)
	while (HL.sensorCol.s4.check(NO_LINE)) { 
		HL.sensorCol.s4.update();
		forwardLineCorrect(1);
	}
	HL.gnd_drive.stop();
	int exit_count  = 0;
	if (anti) {
		HL.nav.bearing = static_cast<Direction>((HL.nav.bearing - exits)%4);
		HL.gnd_drive.turn(-1*TURNING_SPEED);
	} else {
		HL.nav.bearing = static_cast<Direction>((HL.nav.bearing + exits)%4);
		HL.gnd_drive.turn(TURNING_SPEED);
	} //Clockwise or anti-clockwise
	delay(1000); //prevent detecting originating line
	while (exit_count < exits) {
		HL.sensorCol.updateAll();
		bool ok = false;
		if (HL.sensorCol.s1.check(LINE) || HL.sensorCol.s2.check(LINE) || HL.sensorCol.s3.check(LINE)) {
			ok = true;
			if (ok) {
				exit_count++;
				if (exit_count == exits) {
					if (anti) {
						while (HL.sensorCol.s3.check(NO_LINE)){ // wait for the furthest sensor to get in line
							HL.sensorCol.s3.update();
						}
					} else {
						while (HL.sensorCol.s1.check(NO_LINE)){ // wait for the furthest sensor to get in line
							HL.sensorCol.s1.update();
						}
					}
					HL.gnd_drive.stop();
				} else {
					delay(600); //Sensors only detect turn line once while turning
				}
			}
		} else {
			ok = false;
		}
	}
	HL.gnd_drive.forward(FORWARD_SPEED);
}
void Navigation::lossCorrection(bool veer_right) {
	for (int time = 600; time < 2400; time += 400) {
		if(performSweep(time, !veer_right)) {
			break;
		}
	}
}
bool Navigation::performSweep(int time, bool anti) {
	watch.start();
	int start_time = watch.read();
	int speed = 127;
	if (anti) {speed *= -1;}
	HL.gnd_drive.turn(speed);
	while (watch.read() - start_time < time) {
		if (HL.sensorCol.s1.check(LINE) || HL.sensorCol.s2.check(LINE) || HL.sensorCol.s3.check(LINE)) {
			HL.gnd_drive.stop();
			return true;
		}
		HL.sensorCol.updateAll();
	}
	HL.gnd_drive.stop();
	start_time = watch.read();
	HL.gnd_drive.turn(-1*speed);
	while (watch.read() - start_time < 2*time) {
		if (HL.sensorCol.s1.check(LINE) || HL.sensorCol.s2.check(LINE) || HL.sensorCol.s3.check(LINE)) {
			HL.gnd_drive.stop();
			return true;
		}
		HL.sensorCol.updateAll();
	}
	return false;
}

void Navigation::startToRack() {
	junction = 2;
	bearing = NORTH;
	performMove(1,7);
}
	
void Navigation::retrieval() { // to be handled by Pickup class
	junction = 8;
	bearing = SOUTH;
	performMove(1,7);
}

void Navigation::rackToD3() {
	junction = 6;
	bearing = WEST;
	performMove(1,5);
}
			
void Navigation::d3ToRack() {
	junction = 6;
	bearing = EAST;
	performMove(1,7);
}

void Navigation::rackToConveyor() {
	junction = 6;
	bearing = WEST;
	performMove(1,11);
}

void Navigation::conveyorToRack() {
	junction = 4;
	bearing = NORTH;
	performMove(1,7);
}

void Navigation::conveyorToStart() {
	junction = 2;
	bearing = SOUTH;
	performMove(1, 1);
	HL.gnd_drive.stop();
}

void Navigation::d3ToStart() {
	junction = 4;
	bearing = WEST;
	performMove(1, 1);	
	HL.gnd_drive.stop();
}

void Navigation::performLineCorrect(int speed_factor) {
	switch (cur_action) {
	case STRAIGHT:
		//no correction required, continue!
		//cout << "Straight" << endl;
		HL.gnd_drive.forward(FORWARD_SPEED/speed_factor);
		break;
	case RIGHT_VEER:
		//cout << "Will veer Right!" << endl;
		HL.gnd_drive.differential_speed((FORWARD_SPEED + STANDARD_VEER_DEGREE_HIGH) / speed_factor, (FORWARD_SPEED + STANDARD_VEER_DEGREE_LOW) / speed_factor);
		break;
	case LEFT_VEER:
		//cout << "Will veer Left!" << endl;
		HL.gnd_drive.differential_speed((FORWARD_SPEED + STANDARD_VEER_DEGREE_LOW) / speed_factor, (FORWARD_SPEED + STANDARD_VEER_DEGREE_HIGH) / speed_factor);
		break;
	case RIGHT_VEER_LARGE:
		//cout << "Will veer extreme Right!" << endl;
		HL.gnd_drive.differential_speed((FORWARD_SPEED + EXTREME_VEER_DEGREE_HIGH) / speed_factor, (FORWARD_SPEED + EXTREME_VEER_DEGREE_LOW) / speed_factor);
		break;
	case LEFT_VEER_LARGE:
		//cout << "Will veer extreme Left!" << endl;
		HL.gnd_drive.differential_speed((FORWARD_SPEED + EXTREME_VEER_DEGREE_LOW) / speed_factor, (FORWARD_SPEED + EXTREME_VEER_DEGREE_HIGH) / speed_factor);
		break;
	default:
		//undefined cur_action value, proceed as before
		HL.gnd_drive.forward(FORWARD_SPEED / speed_factor);
		break;
	}
}

void Navigation::performMove(int speed_factor, int dest) {
	while (true) {
		HL.gnd_drive.forward(FORWARD_SPEED/speed_factor);
		if (Navigation::Junction_Check() == CROSSROAD) {
			if (handleJunction(dest)) return; //destination reached
		} else { performLineCorrect(speed_factor); }
	}
}

void Navigation::forwardLineCorrect(int speed_factor) {
	HL.gnd_drive.forward(FORWARD_SPEED / speed_factor);
	this->performLineCorrect(speed_factor);
}

bool Navigation::handleJunction(unsigned int dest) {
	//returns true if destination reached, false otherwise
	bool arrived = false;
	if (junction == dest) arrived = true;
	
	cout << "junction = " << junction << endl;
	// the massive switch, need to set the subsequent junction as well
	switch(junction) {
	case 0:
		cout << "junction 0: (70,16)" << endl;
		if (bearing == SOUTH) {
			cout << "stopping" << endl;
			HL.gnd_drive.stop();
		}
		break;
	case 1: 
		cout << "junction 1: S (70,30)" << endl;
		if (bearing == NORTH) {
			cout << "1st retrieve" << endl;
			junction++;
			delay(300);
		} else if (bearing == SOUTH) {
			cout << "ARRIVED HOME! Moving forward to park" << endl;
			delay(500);
			junction--;
		}
		break;
	case 2: 
		cout << "junction 2: (70,44)" << endl;
		delay(500); // prevents(?) detecting same junction again
		if (bearing == NORTH) {
			cout << "1st retrieve" << endl;
			junction++;
			delay(500); // prevents(?) detecting same junction again
		} else if (bearing == SOUTH) {
			junction--;
			delay(500); // prevents(?) detecting same junction again
		}		
		break;
	case 3: 
		cout << "junction 3: (70,120)" << endl;
		if (bearing == NORTH) {
			junction++;
			delay(500);
		} else if (bearing == SOUTH) {
			if (HL.cur_action == NAVIGATING_HOME) {
				junction--;
				delay(500);
			} else if (HL.cur_action == DUMPING_CONVEYOR) {
				junction = 11;
				performTurn(1,false);
			}
		} else if (bearing == WEST) {
			//reversing from conveyor
			if (HL.cur_action == NAVIGATING_HOME) {
				junction--;
				HL.gnd_drive.forward(FORWARD_SPEED);
				performTurn(1,true);
			} else if (HL.cur_action == NAVIGATING_TO_RACK) {
				junction++;
				HL.gnd_drive.forward(FORWARD_SPEED);
				performTurn(1, false);
			}
		}
		break;
	case 4: 
		cout << "junction 4: (70,195)" << endl;
		if (bearing == NORTH) {
			junction++;
			cout << "about to turn" << endl;
			performTurn(1,false);
			cout << "finished turn" << endl;
		} else if (bearing == WEST) {
			junction--;
			performTurn(1,true);
		}	
		break;
	case 5: 
		cout << "junction 5: (150,195)" << endl;
		switch (bearing) {
		case EAST:
			delay(500);
			junction++;
			break;
		case NORTH:
			if (HL.cur_action == NAVIGATING_HOME) {
				//reversing from delivery, go home
				cout <<"turning left";
				junction--;
				HL.gnd_drive.forward(FORWARD_SPEED);
				performTurn(1, true);
			} else if (HL.cur_action == NAVIGATING_TO_RACK) {
				//reversing from delivery, go back to rack
				junction++;
				HL.gnd_drive.forward(FORWARD_SPEED);
				performTurn(1, false);
			}
			break;
		case WEST:
			if (HL.cur_action == DUMPING_D3) {
				cout << "dump D3" << endl;
				junction = 10;
				performTurn(1,false); //dump to D3
			} else if (HL.cur_action == DUMPING_CONVEYOR) {
				cout << "dump conveyor" << endl;
				junction--;
				delay(500); // proceed to conveyor
			}
			break;
		default:
			break;
		}
		break;
	case 6: 
		cout << "junction 6: (170,195)" << endl;
		if (bearing == EAST) {
			junction++;
		} else if (bearing == WEST) {
			junction--;
		}
		delay(500);
		break;
	case 7: 
		cout << "junction 7: (190,195)" << endl;
		if (bearing == EAST) {
			cout << "retrieve: 1st rack" << endl;
			junction++;
			performTurn(1,false);
		} else if (bearing == NORTH) {
			cout << "retrieval complete" << endl;
			junction--;
			performTurn(1,true);
		}		
		break;
	case 8: //to be handled by Pickup class
		cout << "junction 8: (190,120)" << endl;
		if (bearing == SOUTH) {
			junction++;
		} else if (bearing == NORTH) {
			junction--;
		}
		delay(500);
		break;
	case 9: //to be handled by Pickup class
		cout << "junction 9: (190,30)" << endl;
		junction--;
		performTurn(2,false);
		cout << "1st retrieve 2nd rack" << endl;
		break;
	case 10: //to be handled by Dump class
		cout << "junction 10: (150,220)" << endl;
		cout << "stop for dumping" << endl;
		HL.gnd_drive.stop();
		delay(1000); // wait for reverse
		cout << "(reverse)" << endl;
		HL.gnd_drive.forward(-1*FORWARD_SPEED);
		junction = 5;
		break;
	case 11:
		cout << "junction 11: (40,120)" << endl;
		cout << "stop for dumping" << endl;
		break;
	}
	cout << "new bearing = " << bearingToString() << endl;
	return arrived;
}

std::string Navigation::bearingToString() {
	switch (bearing) {
	case NORTH:
		return "NORTH";
	case EAST:
		return "EAST";
	case SOUTH:
		return "SOUTH";
	case WEST:
		return "WEST";
	default:
		return "NULL";
	}
}
