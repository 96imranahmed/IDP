#include "HL.h"

using namespace std;
#include <iostream>
#include <sstream>
#include <string>
#include <robot_delay.h>

HardwareLayer HL;

//HARDWARE LAYER constructor
HardwareLayer::HardwareLayer() {
    isError = false;
    if (!rlink.initialise(ROBOT_NUM)) {
       // error_collection.push_back("Cannot initialise link! Error!");
        rlink.print_errs("    ");
        cout<<"Initialisation Error!"<<endl;
        isError = true;
	} else {
        cout<<"Connected successfully!"<<endl;
    }

    //Sensors
    gnd_left = Sensor_Ground(SENSOR_FARLEFT);
    gnd_right = Sensor_Ground(SENSOR_FARRIGHT);
    gnd_centre = Sensor_Ground(SENSOR_CENTRE);
    rear_left = Sensor_Ground(SENSOR_REARLEFT);
    sensorCol = Sensor_Collection(gnd_left, gnd_centre, gnd_right, rear_left);
    
    //LEDs
    LED_ripe = LED(RIPE_INDICATOR);
    LED_over_ripe = LED(OVER_RIPE_INDICATOR);
    LED_conveyor = LED(CONVEYOR_ENABLE);
    LED_count0 = LED(COUNT0);
    LED_count1 = LED(COUNT1);
    LED_count2 = LED(COUNT2);
    LED_counters = LED_Collection(LED_count0, LED_count1, LED_count2);
    
    //microswitches
    switch_high = Microswitch(OVER_RIPE_DETECT);
    switch_low = Microswitch(RIPE_DETECT);
    switch_conveyor = Microswitch(CONVEYOR);
    
    //driving motors, actuators, arm motor, actions
    gnd_drive = Motor_Ground();
    act_dumper = Actuator(ACTUATOR_DUMPER);
    act_claw = Actuator(ACTUATOR_CLAW);
	arm_motor = Motor_Arm();
	actions = Actions();
}

//modular tests
void test(int TEST_MODULE) {
	switch (TEST_MODULE) {
	case 0:
	HL.gnd_drive.forward(127);
	delay(10000);
		HL.nav.performTurn(1, false);
		break;
	case 1:
		//microswitches
		cout << "Microswitches test: program will wait for each switch to be pressed 5 times." << endl;
		cout << "Now testing lower switch. (bit4)" << endl;
		for (int i = 0; i < 10; i++) {
			while (!HL.switch_low.hasUpdatedHigh()) {}
			cout << "lower switch high; i = " << i << endl;
		}
		cout << "Microswitch test complete." << endl;
		break;
	case 2:
		//LED's
		cout << "LED test: indication LEDs will be toggled on and off 5 times." << endl;
		for (int i = 1; i < 7; i++) {
			HL.LED_over_ripe.setVal(i % 2, false);
			HL.LED_ripe.setVal(i % 2, false);
			HL.LED_conveyor.setVal(i % 2, false);
			cout << "LED's toggled, i = " << i << endl;
			delay(2000);
		}
		cout << "Toggling complete." << endl;

		cout << "LED test: counter LEDs will be display 0-7." << endl;
		for (int i = 0; i <= 7; i++) {
			cout << "i = " << i << endl;;
			HL.LED_counters.setCount(i);
			delay(2000);
		}
		HL.LED_counters.setCount(0);
		cout << "Test complete.";
		break;
	case 3:
		//actuators
		cout << "actuator test: actuators will be toggled on and off 10 times." << endl;
		for (int i = 0; i < 40; i++) {
			cout << "before update: " << HL.act_dumper.toString() << endl;
			switch (i % 4) {
			case 0:
				HL.act_claw.extend();
				cout << "claw extended() (open), i = " << i << endl;
				break;
			case 1:
				HL.act_claw.retract();
				cout << "claw retracted() (closed), i = " << i << endl;
				break;
			case 2:
				HL.act_dumper.extend();
				cout << "dumper extended(), i = " << i << endl;
				break;
			case 3:
				HL.act_dumper.retract();
				cout << "dumper retracted(), i = " << i << endl;
				break;
			}
			delay(4000);
			cout << "after update: " << HL.act_dumper.toString() << endl;
		}
		cout << "actuator test complete. ";
		break;
	case 4:
		//line sensors
		cout << "line sensors test: readings will be read indefinitely." << endl;
		while (true) {
			HL.sensorCol.updateAll();
			cout << HL.sensorCol.toString() << endl;
		}
		break;
	case 5:
		//arm motor
		cout << "arm motor test: arm will move between positions 2 times." << endl
			<< "program will wait for keypress to continue." << endl;
		// d,c,m,u,o
		while (true) {
			cout <<"command: ";
			char c = std::cin.get();
			if (c == 'q') break;
			switch (c) {
			case 'd':
				HL.arm_motor.grab();
				break;
			case 'u':
				HL.arm_motor.dump();
				break;
			case 'o':
				HL.act_claw.extend();
				break;
			case 'c':
				HL.act_claw.retract();
				break;
			case 'x':
				HL.act_dumper.extend();
				break;
			case 'y':
				HL.act_dumper.retract();
				break;
			case 'f': //forward
				HL.gnd_drive.forward(FORWARD_SPEED / 4);
				delay(2000);
				HL.gnd_drive.stop();
				break;
			case 'b': //backward
				HL.gnd_drive.forward(-FORWARD_SPEED / 4);
				delay(2000);
				HL.gnd_drive.stop();
				break;
			}
			
		}
		cout << "arm motor test complete." << endl;
		break;
	case 6:
		cout << "line following start" << endl;
		HL.nav.startToRack();
		HL.nav.retrieval();
		break;
	case 7:
		//move to dumping and reverse
		HL.nav.bearing = WEST;
		HL.cur_action = DUMPING_D3;
		HL.nav.rackToD3(); //(190,195) to (120,220)

		//dump to D3
		HL.cur_action = DUMPING_D3;
		HL.actions.dump.execute();

		//travel to rack
		HL.nav.d3ToRack();
		break;
	case 8:
		HL.act_dumper.retract();
		delay(2000); 
		break;
	case 9:
		// pickup 
		//cout << "line following start" << endl;
		//HL.nav.startToRack();
		cout << "picking test: " << endl;
		HL.cur_action = PICKING_UP_RIPE;
		HL.actions.pickup.execute();
		break;
	case 10:
		//dump
		cout << "dumping test: " << endl;
		HL.cur_action = DUMPING_D3;
		HL.actions.dump.execute();
		break;
	}
}

//Main
int main(int argc, const char * argv[]) {
	HL.rlink.command(RAMP_TIME,0);
	HL.execute();
	//test(2);
	cout << "Test complete, quitting." << endl;
}

#ifdef RIPEFIRST
void HardwareLayer::execute() {
	HL.LED_conveyor.setVal(0, false);
	HL.LED_ripe.setVal(0, false);
	HL.LED_over_ripe.setVal(0, false);
	HL.LED_counters.setCount(0);
	HL.act_dumper.retract();
	
	cout << "******************Travel to rack" << endl;
	HL.cur_action = NAVIGATING_TO_RACK;
	HL.nav.startToRack(); //S to (190,195)
	
	HL.LED_ripe.setVal(1, false);
	HL.LED_over_ripe.setVal(0, false);
	HL.LED_counters.setCount(0);
	cout << "******************Picking up ripe" << endl;
	HL.cur_action = PICKING_UP_RIPE;
	HL.actions.pickup.execute();

	cout << "******************travel to conveyor" << endl;
	HL.nav.rackToConveyor(); //(190,195) to (120,220)

	cout << "******************dump at conveyor" << endl;
	HL.cur_action = DUMPING_CONVEYOR;
	HL.actions.dump.execute();

	cout << "******************travel to rack" << endl;
	HL.nav.conveyorToRack(); //(70,120) to (190,195)

	HL.LED_ripe.setVal(0, false);
	HL.LED_over_ripe.setVal(1, false);
	HL.LED_counters.setCount(0);
	cout << "******************Picking up overripe" << endl;
	HL.cur_action = PICKING_UP_OVERRIPE;
	HL.actions.pickup.execute();

	cout << "******************travel to d3" << endl;
	HL.nav.rackToD3(); //(190,195) to (120,220)

	cout << "******************dump at d3" << endl;
	HL.cur_action = DUMPING_D3;
	HL.actions.dump.execute();

	cout << "******************going home" << endl;
	HL.nav.d3ToStart();
}
#else
void HardwareLayer::execute() {
	cout << "******************Travel to rack" << endl;
	HL.cur_action = NAVIGATING_TO_RACK;
	HL.nav.startToRack(); //S to (190,195)

	cout << "******************Picking up overripe" << endl;
	HL.cur_action = PICKING_UP_OVERRIPE;
	HL.actions.pickup.execute();

	cout << "******************travel to d3" << endl;
	HL.nav.rackToD3(); //(190,195) to (40,120)

	cout << "******************dump at d3" << endl;
	HL.cur_action = DUMPING_D3;
	HL.actions.dump.execute();

	cout << "******************travel to rack" << endl;
	HL.nav.d3ToRack(); //(70,120) to (190,195)

	cout << "******************Picking up ripe" << endl;
	HL.cur_action = PICKING_UP_RIPE;
	HL.actions.pickup.execute();

	cout << "******************travel to conveyor" << endl;
	HL.nav.rackToConveyor(); //(190,195) to (120,220)

	HL.cur_action = DUMPING_CONVEYOR;
	HL.actions.dump.execute();

	cout << "******************going home" << endl;
	HL.nav.conveyorToStart(); //(40,120) to S
}
#endif
