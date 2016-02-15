using namespace std;
#include "HL.h"

#pragma once
#ifndef SENSOR_GROUND_H
#define SENSOR_GROUND_H

extern HardwareLayer HL;

enum Sensor_Val {
	NO_LINE,
	LINE
};
enum Sensor_Type {
	SENSOR_FARLEFT,
	SENSOR_CENTRE,
	SENSOR_FARRIGHT,
	SENSOR_REARLEFT
};

class Sensor_Ground {
public:
    Sensor_Type cur_type;
    Sensor_Val cur_val;
    Sensor_Ground(Sensor_Type gndpos);
    Sensor_Ground() {};    
    void update(int collectionVal);
    void update();
    bool check(Sensor_Val testVal);
private:
	int val;
    int pos;
    Sensor_Val getVal();
};

//Stores all data in one handy class for easy access
class Sensor_Collection {
public:
	Sensor_Ground s1;
	Sensor_Ground s2;
	Sensor_Ground s3;
	Sensor_Ground s4;
	Sensor_Collection(Sensor_Ground s1_in, Sensor_Ground s2_in, Sensor_Ground s3_in, Sensor_Ground s4_in);
	Sensor_Collection() {};
	void updateAll();
	std::string valtoString(Sensor_Ground s);
	std::string toString();
private:
	int val;
};


#endif /*SENSOR_GROUND_H*/
