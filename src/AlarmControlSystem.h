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

    //Variable for storing partial code for setting and resetting
    string partialCode;

    //Variable to store the full code after setting the alarm
    string disarmCode;

    //Flags for controlling the operations of the alarm functions
    bool isArmed;
    bool alarmTriggered;
    bool leftBuildingAfterReset;
    bool requiresAuthorityToDisarm;
    bool disarmTimerStarted;
    bool stopSiren;

    //Storing pointers to all the sensors
    vector<Sensor*> sensors;

    //Creating a timer for allowing the user to exit after arming the system
    cTimer settingAlarmTimer;
    uint32_t settingAlarmTimerSec = SETTING_ALARM_TIME;
    uint32_t settingAlarmTimerMsec = 0;

    //Creating a timer for triggering the alarm after triggering the sensor
    cTimer triggeringAlarmTimer;
    uint32_t triggeringAlarmTimerSec = TIME_BETWEEN_DETECTING_AN_INTRUDER_AND_TRIGGERING_ALARM;
    uint32_t triggeringAlarmTimerMsec = 0;

    //Creating a timer to allow the user to reenter the building and disarm the alarm
    cTimer reentryTimer;
    uint32_t reentryTimerSec = TIME_ALLOWED_FOR_REENTRY;
    uint32_t reentryTimerMsec = 0;

    //Creating a timer to allow the user to disarm the system by entering the correct code in the specified time
    cTimer disarmTimer;
    uint32_t disarmTimerSec = RESETTING_ALARM_TIME;
    uint32_t disarmTimerMsec = 0;

    //Creating a timer to control the time the code is fully entered
    cTimer codeEntryTimer;
    uint32_t codeEntryTimerSec = MAXIMUM_DURATION_FOR_ENTERING_THE_CODE;
    uint32_t codeEntryTimerMsec = 0;

    //Timer for controlling the siren
    cTimer sirenDurationTimer;
    uint32_t sirenDurationTimerSec = SIREN_DURATION;
    uint32_t sirenDurationTimerMsec = 0;

    //Threads to manage each timer independently
    thread settingAlarmThread;
    thread triggeringAlarmThread;
    thread reentryTimerThread;
    thread disarmTimerThread;
    thread codeEntryTimerThread;
    thread sirenDurationTimerThread;

public:
    AlarmControlSystem();

    void armSystem(const char digit);
    void disarmSystem(const char digit);

    void checkSensors();
    void triggerAlarm();
    void turnOffAlarm();

    void addSensor(Sensor* sensor);

    void resetSystem();

    void handleSirenTimer();

    void startTimerThreads();
    void joinTimerThreads();

    void authorityDisarmSystem();

};

#endif //AN_INTRUDER_ALARM_ALARMCONTROLSYSTEM_H
