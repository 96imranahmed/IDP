using namespace std;

#include "Actions.h"
#include "Navigation.h"
#include <stopwatch.h>

void Pickup::initialise() {
	switch(HL.cur_action) {
	case PICKING_UP_RIPE: 
	case PICKING_UP_OVERRIPE:
		break;
	default:
		cout << "invalid Action parameter for Pickup" << endl;
		break;
	}
	HL.actions.num_taken = 0;
	stopwatch watch;
}

void Pickup::execute() {
	this->initialise();
	
	/* calls move() four times
	 * move returns when a junction is encountered
	 */
	 //from junction (190,195)...
	this->move(true); 
	//junction (190,120), do nothing.
	cout <<"middle junction! 1st" << endl;
	HL.nav.junction++;

	this->move(false); 
	//junction (190,30), do u-turn and move on to second rack.
	cout << "end junction!";
	HL.nav.performTurn(2, true);
	this->move(true);
	
	//junction (190,120), do nothing.
	cout <<"middle junction! 2nd" << endl;
	this->move(false);
	
	cout <<"end of pickup!";
	//junction (190,195), retract arm!
	HL.arm_motor.dump(); //move to top position for driving
	
	cout <<"now turning!" << endl;
	
	if (HL.cur_action == PICKING_UP_RIPE) {
		HL.cur_action = DUMPING_CONVEYOR;
	} else if (HL.cur_action == PICKING_UP_OVERRIPE) {
		HL.cur_action = DUMPING_D3;
	}
	HL.nav.performTurn(1, true);
}
void Pickup::realign() {
	HL.sensorCol.updateAll();
	if (HL.sensorCol.s1.check(NO_LINE) || HL.sensorCol.s3.check(NO_LINE)) {
	HL.gnd_drive.stop();
	if (HL.sensorCol.s1.check(LINE)) {
		HL.gnd_drive.turn(-80);
		while (!(HL.sensorCol.s2.check(LINE) && HL.sensorCol.s3.check(NO_LINE))) {
			HL.sensorCol.updateAll();
		}
		//Swing left
	} else {
		HL.gnd_drive.turn(80);
				while (!(HL.sensorCol.s2.check(LINE) && HL.sensorCol.s1.check(NO_LINE))) {
		HL.sensorCol.updateAll();
		}
	}
	HL.gnd_drive.stop();
	HL.gnd_drive.forward(70);
	}
}

void Pickup::move(bool ignoreJunction) {
	HL.act_claw.extend();
	HL.act_dumper.retract();
	while (HL.nav.Junction_Check() != CROSSROAD) {
		//as long as its not a junction, drive forward
		HL.nav.forwardLineCorrect(1.5);
		//try to detect for right spear
		bool check;
		switch (HL.cur_action) {
		case PICKING_UP_OVERRIPE:
			if (HL.switch_high.hasUpdatedHigh()) {
				realign();
				delay(350);
				HL.gnd_drive.stop();
				HL.gnd_drive.reverse(50);
				delay(250);
				HL.gnd_drive.stop();
				HL.LED_over_ripe.setVal(1,false);
				check = grab();
				HL.LED_over_ripe.setVal(0,false);
				if (!check) realign();
				if ((HL.sensorCol.s1.check(LINE) && HL.sensorCol.s2.check(LINE) && HL.sensorCol.s3.check(LINE)) || check) {
					watch.start();
					while (watch.read() < 500) {
						HL.nav.Junction_Check();
						HL.nav.performLineCorrect(2);
					}
					return;
				}
			}
			break;
		case PICKING_UP_RIPE:
			if (HL.switch_low.hasUpdatedHigh() && !HL.switch_high.hasUpdatedHigh()) {//found a spear
				realign();
				delay(300);
				HL.gnd_drive.stop();
				HL.gnd_drive.reverse(50);
				delay(250);
				HL.gnd_drive.stop();
				HL.LED_ripe.setVal(1,false);
				check = grab();
				HL.LED_ripe.setVal(0,false);
				if (!check) realign();
				if ((HL.sensorCol.s1.check(LINE)&& HL.sensorCol.s2.check(LINE) && HL.sensorCol.s3.check(LINE)) || check) {
					watch.start();
					while (watch.read() < 500) {
						HL.nav.Junction_Check();
						HL.nav.performLineCorrect(2);
					}
					return;
				}
			}
			break;
		default: 
			break;
		}
	}
	watch.start();
	while (watch.read() < 500) {
		HL.nav.Junction_Check();
		HL.nav.performLineCorrect(2);
	}
}

bool Pickup::grab() {
	HL.act_claw.extend(); //extend actuator to open claws
	HL.arm_motor.grab(); //move arm to grab position
	HL.act_claw.retract(); //retract actuator to close claws
	bool checkJunc = false;
	//drive forward slightly to lift asparagus off root
	watch.start();
	HL.gnd_drive.forward(FORWARD_SPEED/2);
	while (watch.read() < 750) {
		if (HL.nav.Junction_Check() == CROSSROAD) checkJunc = true;
	}
	HL.gnd_drive.stop();
	HL.arm_motor.dump(); //move arm to dump position
	HL.act_claw.extend(); //extend actuator to open claws (to drop asparagus)
	HL.LED_counters.setCount(++HL.actions.num_taken);
	cout <<"num_taken = " << HL.actions.num_taken << endl;
	return checkJunc;
}

void Dump::initialise() {
	switch (HL.cur_action) {
	case DUMPING_D3:
	case DUMPING_CONVEYOR:
		break;
	default:
		//invalid action for Dump
		cout << "invalid Action parameter for Dump" << endl;
		break;
	}
}

void Dump::execute() {
	this->initialise();
	HL.nav.forwardLineCorrect(1);
	// drive until microswitch bumps into conveyor
	if (HL.cur_action == DUMPING_CONVEYOR) {
		while (!HL.switch_conveyor.hasUpdatedHigh()) {
			HL.nav.forwardLineCorrect(1);	
		}
		cout <<"here";
		HL.LED_ripe.setVal(1, false);
	} else if (HL.cur_action == DUMPING_D3) { //drive until red area detected
		while (HL.nav.Junction_Check() != CROSSROAD) {
			HL.nav.forwardLineCorrect(1);	
		}
		HL.LED_over_ripe.setVal(1, false);
	}
	
	cout <<"stopped at delivery";
	//indicate number to be dumped
	HL.LED_counters.setCount(HL.actions.num_taken);
	
	HL.gnd_drive.stop(); // stop for dumping
	HL.act_dumper.extend();	// dump!!
	delay(5000); //give it time
	HL.act_dumper.retract();

// done, now reverse and resume navigation	
	delay(6000);
	//turn off all the LEDs
	HL.LED_counters.setCount(0);
	if (HL.cur_action == DUMPING_CONVEYOR) {
		HL.LED_ripe.setVal(0, false);
	} else if (HL.cur_action == DUMPING_D3) {
		HL.LED_over_ripe.setVal(0, false);
	}
	
	int i = 0;
	
	HL.gnd_drive.reverse(FORWARD_SPEED);
	delay(800);
	if (HL.cur_action == DUMPING_D3) {
		while ((HL.nav.Junction_Check() != CROSSROAD)  && i < 20) {
			HL.nav.forwardLineCorrect(-1);
			cout << "i = " << i <<endl;
			i++;
		}
#ifdef RIPEFIRST
		HL.cur_action = NAVIGATING_HOME;
		HL.nav.performTurn(1,true);
#else
		HL.cur_action = NAVIGATING_TO_RACK;
		HL.nav.performTurn(1,false);
#endif
	} else if (HL.cur_action == DUMPING_CONVEYOR){
		while ((HL.nav.Junction_Check() != CROSSROAD)&& i < 5) {
			HL.nav.forwardLineCorrect(-1);
			cout << "i = " << i <<endl;
			i++;
		}
		cout <<"reached junction 11;" <<endl;
		
		HL.gnd_drive.reverse(FORWARD_SPEED);
		while (HL.sensorCol.s4.check(NO_LINE)) {
			cout <<"waiting for junction 3" <<endl;
			HL.nav.forwardLineCorrect(-1);
			HL.sensorCol.s4.update();
		}
#ifdef RIPEFIRST
		HL.cur_action = NAVIGATING_TO_RACK;
		cout <<"about to turn right";
		HL.nav.performTurn(1,false);
#else
		HL.cur_action = NAVIGATING_HOME;
		cout <<"about to turn left";
		HL.nav.performTurn(1,true);
#endif
	}

}
