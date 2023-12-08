#include "Sensor.h"

Sensor::Sensor() {
    isTriggered = false;
    type = "";
}

Sensor::Sensor(string type) {
    isTriggered = false;
    this->type = type;
}

void Sensor::trigger() {
    isTriggered = true;
}

void Sensor::reset() {
    isTriggered = false;
}

bool Sensor::getState() {
    return isTriggered;
}

void Sensor::setType(string type) {
    this->type = type;
}

string Sensor::getType() {
    return type;
}
