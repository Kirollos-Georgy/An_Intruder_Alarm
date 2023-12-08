#include "AlarmControlSystem.h"

AlarmControlSystem::AlarmControlSystem()
    : isArmed(false),
      alarmTriggered(false),
      disarmCode(""),
      leftBuildingAfterReset(false),
      triggeringAlarmTimer(triggeringAlarmTimerSec, triggeringAlarmTimerMsec),
      settingAlarmTimer(settingAlarmTimerSec, settingAlarmTimerMsec),
      reentryTimer(reentryTimerSec, reentryTimerMsec),
      disarmTimer(disarmTimerSec, disarmTimerMsec) {
}

void AlarmControlSystem::armSystem(const string& disarmCode) {
    if(!isArmed) {
        this->disarmCode = disarmCode;
        isArmed = true;
        alarmTriggered = false;
        settingAlarmTimer.startSingleShotTimer(SETTING_ALARM_TIME, 0);
        cout << "Starting timer for setting the alarm (30 seconds)" << endl;
        cout << "Alarm is armed (set)" << endl;
    }
}

void AlarmControlSystem::disarmSystem(const string& disarmCode) {

	if (!isArmed) {
		return;
	}

	if (!disarmTimer.isRunning()) {
		disarmTimer.startSingleShotTimer(RESETTING_ALARM_TIME, 0);
		cout << "Starting timer for disarming the alarm (60 seconds)" << endl;

	}

	if (!disarmTimer.isExpired()) {
		if (this->disarmCode == disarmCode) {
			isArmed = false;
			alarmTriggered = false;
			settingAlarmTimer.stopTimer();
			triggeringAlarmTimer.stopTimer();
			reentryTimer.stopTimer();
			disarmTimer.stopTimer();
			cout << "Alarm is disarmed (reset)" << endl;
		} else {
			cout << "Incorrect disarm code" << endl;
		}
	} else {
		triggerAlarm();
	}
}

void AlarmControlSystem::checkSensors() {
    if (!isArmed) {
        return;
    }

    bool isNominatedExitTriggered = false;

    for (auto& sensor : sensors) {
        if (sensor->getState()) {
            if (sensor->getType() == "nominatedExit") {
                isNominatedExitTriggered = true;
                cout << "Sensor at nominated exit is triggered" << endl;
                if (!settingAlarmTimer.isExpired()) {
                    leftBuildingAfterReset = true;
                    cout << "User left the building in time" << endl;
                }
                else if (!leftBuildingAfterReset) {
                    if (!alarmTriggered) {
                    	triggerAlarm();
                        break;
                    }
                } else if (!reentryTimer.isRunning()) {
                	cout << "User entered the building from the nominated entry point" << endl;
                    reentryTimer.startSingleShotTimer(TIME_ALLOWED_FOR_REENTRY, 0);
                    cout << "Starting timer for re-entry from the nominated exit (120 seconds)" << endl;
                }
            } else {
                if (!alarmTriggered) {
                	cout << "A sensor is triggered" << endl;
                    triggeringAlarmTimer.startSingleShotTimer(TIME_BETWEEN_DETECTING_AN_INTRUDER_AND_TRIGGERING_ALARM, 0); // Start the triggering alarm timer
                    cout << "Starting timer for triggering the alarm (5 seconds)" << endl;
                    break;
                }
            }
        }
    }

    if (isNominatedExitTriggered && leftBuildingAfterReset && settingAlarmTimer.isExpired() && !reentryTimer.isRunning()) {
        leftBuildingAfterReset = false;
    }

    if (triggeringAlarmTimer.isExpired() && !alarmTriggered) {
        triggerAlarm();
    }

    if (reentryTimer.isExpired() && isArmed) {
        triggerAlarm();
    }
}

void AlarmControlSystem::triggerAlarm() {
    if (!alarmTriggered) {
        alarmTriggered = true;
        cout << "Alarm triggered" << endl;
    }
}

void AlarmControlSystem::addSensor(Sensor* sensor) {
    sensors.push_back(sensor);
}

void AlarmControlSystem::resetSystem() {
	isArmed = false;
	alarmTriggered = false;
	disarmCode = "";
	leftBuildingAfterReset = false;
	triggeringAlarmTimer.stopTimer();
	settingAlarmTimer.stopTimer();
	reentryTimer.stopTimer();
	disarmTimer.stopTimer();
}

void AlarmControlSystem::startTimerThreads() {
    settingAlarmThread = thread([this](){ this->settingAlarmTimer.waitTimer(); });
    triggeringAlarmThread = thread([this](){ this->triggeringAlarmTimer.waitTimer(); });
    reentryTimerThread = thread([this](){ this->reentryTimer.waitTimer(); });
    disarmTimerThread = thread([this](){ this->disarmTimer.waitTimer(); });
}

void AlarmControlSystem::joinTimerThreads() {
    if (settingAlarmThread.joinable()) settingAlarmThread.join();
    if (triggeringAlarmThread.joinable()) triggeringAlarmThread.join();
    if (reentryTimerThread.joinable()) reentryTimerThread.join();
    if (disarmTimerThread.joinable()) disarmTimerThread.join();
}
