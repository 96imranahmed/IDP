using namespace std;
#include "Sensor_ground.h"

//constructor
Sensor_Ground::Sensor_Ground(Sensor_Type gndpos) {
	cur_type = gndpos;
	switch (gndpos) {
	case SENSOR_FARLEFT:
		pos = 1<<0;
		break;
	case SENSOR_CENTRE:
		pos = 1<<1;
		break;
	case SENSOR_FARRIGHT:
		pos = 1<<2;
		break;
	case SENSOR_REARLEFT:
		pos = 1<<3;
		break;
	}
}

void Sensor_Ground::update(int collectionVal) {
	val = collectionVal;
	cur_val = getVal();
}

void Sensor_Ground::update() {
	val = HL.rlink.request(READ_PORT_0);
	cur_val = getVal();
}

bool Sensor_Ground::check(Sensor_Val testVal){
	return (getVal() == testVal);
}

Sensor_Val Sensor_Ground::getVal() {
	return static_cast<Sensor_Val>((val & pos)/pos);
}

Sensor_Collection::Sensor_Collection(Sensor_Ground s1_in, Sensor_Ground s2_in, Sensor_Ground s3_in, Sensor_Ground s4_in) {
    s1 = s1_in;
    s2 = s2_in;
    s3 = s3_in;
    s4 = s4_in;
}

void Sensor_Collection::updateAll() {
	val = HL.rlink.request(READ_PORT_0);
    s1.update(val);
    s2.update(val);
    s3.update(val);
    s4.update(val);
}	

std::string Sensor_Collection::valtoString(Sensor_Ground s) {
    if (s.check(NO_LINE)) {
        return "No Line";
	} else {
        return "Line";
    }
}
std::string Sensor_Collection::toString() {
    return (valtoString(s1) + " " + valtoString(s2) + " " + valtoString(s3) + " " + valtoString(s4));
}
