#ifndef AN_INTRUDER_ALARM_SENSOR_H
#define AN_INTRUDER_ALARM_SENSOR_H

#include <iostream>

using namespace std

class Sensor {
private:
    bool isTriggered;
    string type;

public:
    Sensor();
    Sensor(String type);

    void trigger();
    void reset();
    bool getState();

    void setType(string type);
    string getType();
};
#endif //AN_INTRUDER_ALARM_SENSOR_H
