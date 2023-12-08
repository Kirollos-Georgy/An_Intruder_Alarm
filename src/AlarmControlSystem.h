#ifndef AN_INTRUDER_ALARM_ALARMCONTROLSYSTEM_H
#define AN_INTRUDER_ALARM_ALARMCONTROLSYSTEM_H

#include <iostream>
#include <vector>
#include <string>

#include "cTimer.h"
#include "Sensor.h"

using namespace std;

class AlarmControlSystem {
private:
    // Constant timing parameters (in seconds)
    const int SETTING_ALARM_TIME = 30;
    const int TIME_BETWEEN_DETECTING_AN_INTRUDER_AND_TRIGGERING_ALARM = 5;
    const int TIME_ALLOWED_FOR_REENTRY = 2*60;
    const int RESETTING_ALARM_TIME = 60;
    const int MAXIMUM_DURATION_FOR_ENTERING_THE_CODE = 20;
    const int SIREN_DURATION = 5*60;

    bool isArmed;
    bool alarmTriggered;
    bool leftBuildingAfterReset;
    string disarmCode;
    vector<Sensor*> sensors;

    cTimer settingAlarmTimer;
    uint32_t settingAlarmTimerSec = SETTING_ALARM_TIME;
    uint32_t settingAlarmTimerMsec = 0;

    cTimer triggeringAlarmTimer;
    uint32_t triggeringAlarmTimerSec = TIME_BETWEEN_DETECTING_AN_INTRUDER_AND_TRIGGERING_ALARM;
    uint32_t triggeringAlarmTimerMsec = 0;

    cTimer reentryTimer;
    uint32_t reentryTimerSec = TIME_ALLOWED_FOR_REENTRY;
    uint32_t reentryTimerMsec = 0;

    cTimer disarmTimer;
    uint32_t disarmTimerSec = RESETTING_ALARM_TIME;
    uint32_t disarmTimerMsec = 0;

    thread settingAlarmThread;
    thread triggeringAlarmThread;
    thread reentryTimerThread;
    thread disarmTimerThread;

public:
    AlarmControlSystem();

    void armSystem(const string& disarmCode);
    void disarmSystem(const string& disarmCode);

    void checkSensors();
    void triggerAlarm();

    void addSensor(Sensor* sensor);

    void resetSystem();

    void startTimerThreads();
    void joinTimerThreads();

};

#endif //AN_INTRUDER_ALARM_ALARMCONTROLSYSTEM_H
