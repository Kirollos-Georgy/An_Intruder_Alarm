#include "AlarmControlSystem.h"

AlarmControlSystem::AlarmControlSystem()
    : isArmed(false),
      alarmTriggered(false),
      disarmCode(""),
      leftBuildingAfterReset(false),
	  requiresAuthorityToDisarm(false),
	  disarmTimerStarted(false),
      triggeringAlarmTimer(triggeringAlarmTimerSec, triggeringAlarmTimerMsec),
      settingAlarmTimer(settingAlarmTimerSec, settingAlarmTimerMsec),
      reentryTimer(reentryTimerSec, reentryTimerMsec),
      disarmTimer(disarmTimerSec, disarmTimerMsec),
	  codeEntryTimer(codeEntryTimerSec, codeEntryTimerMsec),
	  sirenDurationTimer(sirenDurationTimerSec, sirenDurationTimerMsec) {
}

void AlarmControlSystem::armSystem(const char digit) {
    if(!isArmed) {
    	if (partialCode.empty()) {
    	    codeEntryTimer.startSingleShotTimer(codeEntryTimerSec, codeEntryTimerMsec);
    	    cout << "Starting timer for code entry (20 seconds)" << endl;
    	}

    	partialCode += digit;

    	if (codeEntryTimer.isExpired()) {
    		partialCode.clear();
    		cout << "Duration for code entry has expired, please re-enter the code" << endl;
    		return;
    	}

    	if (partialCode.length() == 4) {
    		 disarmCode = partialCode;;
    		 isArmed = true;
    		 alarmTriggered = false;
    		 settingAlarmTimer.startSingleShotTimer(SETTING_ALARM_TIME, 0);
    		 cout << "Alarm is armed (set)" << endl;
    		 cout << "Starting timer for setting the alarm (30 seconds)" << endl;

    		 partialCode.clear();
    		 codeEntryTimer.stopTimer();
    	}
    }
}

void AlarmControlSystem::disarmSystem(const char digit) {

	if (!isArmed) {
		return;
	}

	if (requiresAuthorityToDisarm) {
		cout << "System requires authority to disarm." << endl;
		return;
	}

	if (!disarmTimer.isRunning() && !requiresAuthorityToDisarm && !disarmTimerStarted) {
		disarmTimer.startSingleShotTimer(RESETTING_ALARM_TIME, 0);
		cout << "Starting timer for disarming the alarm (60 seconds)" << endl;
	    disarmTimerStarted = true;
	}


	if (partialCode.empty()) {
		codeEntryTimer.startSingleShotTimer(codeEntryTimerSec, codeEntryTimerMsec);
		cout << "Starting timer for code entry (20 seconds)" << endl;
	}

	partialCode += digit;

	if (codeEntryTimer.isExpired()) {
		partialCode.clear();
		cout << "Duration for code entry has expired, please re-enter the code" << endl;
	    return;
	}

	if (disarmTimer.isExpired() && !alarmTriggered) {
		cout << "Disarm timer expired, system requires authority to disarm." << endl;
	    requiresAuthorityToDisarm = true;
	    triggerAlarm();
	    return;
	}

	if (!disarmTimer.isExpired()) {
		if (partialCode.length() == 4) {
			if (disarmCode == partialCode) {
				isArmed = false;
				alarmTriggered = false;
			    disarmTimerStarted = true;
				settingAlarmTimer.stopTimer();
				triggeringAlarmTimer.stopTimer();
				reentryTimer.stopTimer();
				disarmTimer.stopTimer();
				codeEntryTimer.stopTimer();
				sirenDurationTimer.stopTimer();
				cout << "Alarm is disarmed (reset)" << endl;
			} else {
				cout << "Incorrect disarm code" << endl;
			}
			partialCode.clear();
			codeEntryTimer.stopTimer();
		}
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
                	if (!leftBuildingAfterReset) {
                		cout << "User left the building in time through the nominated exit" << endl;
                		leftBuildingAfterReset = true;
                	}
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
        sirenDurationTimer.startSingleShotTimer(sirenDurationTimerSec, sirenDurationTimerMsec);
        cout << "Siren and siren timer (5 minutes) have started" << endl;
        cout << "Strobe has started" << endl;

        std::thread sirenThread(&AlarmControlSystem::handleSirenTimer, this);
        sirenThread.detach();
    }
}

void AlarmControlSystem::handleSirenTimer() {
    while (alarmTriggered) {
        sirenDurationTimer.waitTimer();

        if (sirenDurationTimer.isExpired()) {
            cout << "Siren has stopped" << endl;
            break;
        }
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
	requiresAuthorityToDisarm = false;
    disarmTimerStarted = false;

	triggeringAlarmTimer.stopTimer();
	settingAlarmTimer.stopTimer();
	reentryTimer.stopTimer();
	disarmTimer.stopTimer();
	codeEntryTimer.stopTimer();
	sirenDurationTimer.stopTimer();

	for (auto& sensor : sensors) {
		sensor->reset();
	}
}

void AlarmControlSystem::startTimerThreads() {
    settingAlarmThread = thread([this](){ this->settingAlarmTimer.waitTimer(); });
    triggeringAlarmThread = thread([this](){ this->triggeringAlarmTimer.waitTimer(); });
    reentryTimerThread = thread([this](){ this->reentryTimer.waitTimer(); });
    disarmTimerThread = thread([this](){ this->disarmTimer.waitTimer(); });
    codeEntryTimerThread = thread([this](){ this->codeEntryTimer.waitTimer(); });
    sirenDurationTimerThread = thread([this](){ this->sirenDurationTimer.waitTimer(); });
}

void AlarmControlSystem::joinTimerThreads() {
    if (settingAlarmThread.joinable()) settingAlarmThread.join();
    if (triggeringAlarmThread.joinable()) triggeringAlarmThread.join();
    if (reentryTimerThread.joinable()) reentryTimerThread.join();
    if (disarmTimerThread.joinable()) disarmTimerThread.join();
    if (codeEntryTimerThread.joinable()) codeEntryTimerThread.join();
    if (sirenDurationTimerThread.joinable()) sirenDurationTimerThread.join();
}

void AlarmControlSystem::authorityDisarmSystem() {
    if (requiresAuthorityToDisarm) {
        isArmed = false;
        alarmTriggered = false;
        requiresAuthorityToDisarm = false;
    	triggeringAlarmTimer.stopTimer();
    	settingAlarmTimer.stopTimer();
    	reentryTimer.stopTimer();
    	disarmTimer.stopTimer();
    	codeEntryTimer.stopTimer();
    	sirenDurationTimer.stopTimer();
        cout << "Alarm disarmed by authority." << endl;
    }
}
