#include "Sensor.h"

/*
 * Default constructor
 */
Sensor::Sensor() {
    isTriggered = false;
    type = "";
}

Sensor::Sensor(string type) {
    isTriggered = false;
    this->type = type;
}

/*
 * Triggering the sensor
 */
void Sensor::trigger() {
    isTriggered = true;
}

/*
 * Reseting the sensor
 */
void Sensor::reset() {
    isTriggered = false;
}

/*
 * Getting sensor state
 */
bool Sensor::getState() {
    return isTriggered;
}

/*
 * Setting sensor type (sensor name/location)
 */
void Sensor::setType(string type) {
    this->type = type;
}

/*
 * Getting sensor type
 */
string Sensor::getType() {
    return type;
}
