#include "AlarmControlSystem.h"

/*
 * Initializing the variables
 */
AlarmControlSystem::AlarmControlSystem()
        : isArmed(false),
          alarmTriggered(false),
          disarmCode(""),
          leftBuildingAfterReset(false),
          requiresAuthorityToDisarm(false),
          disarmTimerStarted(false),
		  stopSiren(true),
          triggeringAlarmTimer(triggeringAlarmTimerSec, triggeringAlarmTimerMsec),
          settingAlarmTimer(settingAlarmTimerSec, settingAlarmTimerMsec),
          reentryTimer(reentryTimerSec, reentryTimerMsec),
          disarmTimer(disarmTimerSec, disarmTimerMsec),
          codeEntryTimer(codeEntryTimerSec, codeEntryTimerMsec),
          sirenDurationTimer(sirenDurationTimerSec, sirenDurationTimerMsec) {
}

/*
 * Function to arm the system
 */
void AlarmControlSystem::armSystem(const char digit) {
    if (!isArmed) { //Checking if the system is not armed
        if (partialCode.empty()) {
            //Starting the code entry timer when entering the first digit of the code
            codeEntryTimer.startSingleShotTimer(codeEntryTimerSec, codeEntryTimerMsec);
            cout << "Starting timer for code entry (20 seconds)" << endl;
        }

        partialCode += digit;

        //Clearing the code entered if the code entry timer has expired
        if (codeEntryTimer.isExpired()) {
            partialCode.clear();
            cout << "Duration for code entry has expired, please re-enter the code" << endl;
            return;
        }

        //If the code is complete
        if (partialCode.length() == 4) {
            //Store the code
            disarmCode = partialCode;;
            isArmed = true;
            alarmTriggered = false;
            //Start the timer for allowing the user to exit the building
            settingAlarmTimer.startSingleShotTimer(settingAlarmTimerSec, settingAlarmTimerMsec);
            cout << "Alarm is armed (set)" << endl;
            cout << "Starting timer for setting the alarm (30 seconds)" << endl;

            //Clearing the code for the next time arming the system
            partialCode.clear();

            //Stop the code entry timer
            codeEntryTimer.stopTimer();
        }
    }
}

/*
 * Function for disarming the system
 */
void AlarmControlSystem::disarmSystem(const char digit) {

    //Don't do anything if the system is not armed
    if (!isArmed) {
        return;
    }

    //If the disarm timer has expired, an authority needs to disarm the system
    if (requiresAuthorityToDisarm) {
        cout << "System requires authority to disarm." << endl;
        return;
    }

    //If the disarm timer is not running, the system doesn't need an authority doesn't need to disarm the system and the disarm timer wasn't running
    if (!disarmTimer.isRunning() && !requiresAuthorityToDisarm && !disarmTimerStarted) {
        //Start the disarm timer
        disarmTimer.startSingleShotTimer(disarmTimerSec, disarmTimerMsec);
        cout << "Starting timer for disarming the alarm (60 seconds)" << endl;
        disarmTimerStarted = true;
    }

    //When the first digit of the code is entered
    if (partialCode.empty()) {
        //Start the code entry timer
        codeEntryTimer.startSingleShotTimer(codeEntryTimerSec, codeEntryTimerMsec);
        cout << "Starting timer for code entry (20 seconds)" << endl;
    }

    partialCode += digit;

    //If the code entry timer has expired
    if (codeEntryTimer.isExpired()) {
        //Clear the code entered
        partialCode.clear();
        cout << "Duration for code entry has expired, please re-enter the code" << endl;
        return;
    }

    //If the disarm timer has expired
    if (disarmTimer.isExpired() && !alarmTriggered) {
        //The system now will need to be disarmed with an independent authority only
        cout << "Disarm timer expired, system requires authority to disarm." << endl;
        requiresAuthorityToDisarm = true;
        //trigger the alarm
        triggerAlarm();
        return;
    }

    //If the disarm timer didn't expire
    if (!disarmTimer.isExpired()) {
        //if the code entered is complete
        if (partialCode.length() == 4) {
            if (disarmCode == partialCode) {
                isArmed = false;
                disarmTimerStarted = true;
                //Stop all timers
                settingAlarmTimer.stopTimer();
                triggeringAlarmTimer.stopTimer();
                reentryTimer.stopTimer();
                disarmTimer.stopTimer();
                codeEntryTimer.stopTimer();
                sirenDurationTimer.stopTimer();
                cout << "Alarm is disarmed (reset)" << endl;
                turnOffAlarm();
                alarmTriggered = false;
            } else {
                cout << "Incorrect disarm code" << endl;
            }
            //Clear the entered code for the next time
            partialCode.clear();

            //Stop the code entry timer
            codeEntryTimer.stopTimer();
        }
    }
}

/*
 * Function to check the status of each sensor
 */
void AlarmControlSystem::checkSensors() {
    //Don't check the sesnor status if the system is not armed
    if (!isArmed) {
        return;
    }

    bool isNominatedExitTriggered = false;

    //Looping through all the sensors
    for (auto &sensor: sensors) {
        //Checking if the sensor is triggered
        if (sensor->getState()) {
            if (sensor->getType() == "nominatedExit") {
                isNominatedExitTriggered = true;
                cout << "Sensor at nominated exit is triggered" << endl;
                if (!settingAlarmTimer.isExpired()) {
                    if (!leftBuildingAfterReset) {
                        cout << "User left the building in time through the nominated exit" << endl;
                        leftBuildingAfterReset = true;
                        break;
                    }
                }
                //If the user didn't leave the building in time
                else if (!leftBuildingAfterReset) {
                    if (!alarmTriggered) {
                        triggerAlarm();
                        break;
                    }
                } else if (!reentryTimer.isRunning()) {
                    cout << "User entered the building from the nominated entry point" << endl;
                    reentryTimer.startSingleShotTimer(reentryTimerSec, reentryTimerMsec);
                    cout << "Starting timer for re-entry from the nominated exit (120 seconds)" << endl;
                    break;
                }
            } else {
                if (!alarmTriggered) {
                    cout << "A sensor is triggered" << endl;
                    // Start the triggering alarm timer
                    triggeringAlarmTimer.startSingleShotTimer(triggeringAlarmTimerSec, triggeringAlarmTimerMsec);
                    cout << "Starting timer for triggering the alarm (5 seconds)" << endl;
                    break;
                }
            }
        }
    }

    if (isNominatedExitTriggered && leftBuildingAfterReset && settingAlarmTimer.isExpired() &&
        !reentryTimer.isRunning()) {
        leftBuildingAfterReset = false;
    }

    if (triggeringAlarmTimer.isExpired() && !alarmTriggered) {
        triggerAlarm();
    }

    if (reentryTimer.isExpired() && isArmed) {
        triggerAlarm();
    }
}

/*
 * Method for triggering the alarm
 */
void AlarmControlSystem::triggerAlarm() {
    //If the alarm is not triggered
    if (!alarmTriggered) {
        //Trigger the alarm
        alarmTriggered = true;
        stopSiren = true;
        cout << "Alarm triggered" << endl;
        //Start the siren timer
        sirenDurationTimer.startSingleShotTimer(sirenDurationTimerSec, sirenDurationTimerMsec);
        cout << "Siren and siren timer (5 minutes) have started" << endl;
        cout << "Strobe has started" << endl;

        //Start a thread to check the siren timer
        thread sirenThread(&AlarmControlSystem::handleSirenTimer, this);
        sirenThread.detach();
    }
}

/*
 * Method to turn off the alarm
 */
void AlarmControlSystem::turnOffAlarm() {
	if (alarmTriggered) {
		if (stopSiren) {
			cout << "Siren has stopped" << endl;
			sirenDurationTimer.stopTimer();
		}
		cout << "Strobe has stopped" << endl;
	}
}

/*
 * Method to stop the siren after 5 minutes
 */
void AlarmControlSystem::handleSirenTimer() {
    while (alarmTriggered) {
        sirenDurationTimer.waitTimer();

        //When the siren timer expired, turn off the siren
        if (sirenDurationTimer.isExpired()) {
            cout << "Siren has stopped" << endl;
            stopSiren = false;
            break;
        }
    }
}

/*
 * Method for adding pointers to sensors to the system
 */
void AlarmControlSystem::addSensor(Sensor *sensor) {
    sensors.push_back(sensor);
}

/*
 * Function to reset the system to the initial state
 */
void AlarmControlSystem::resetSystem() {
    isArmed = false;
    alarmTriggered = false;
    disarmCode = "";
    leftBuildingAfterReset = false;
    requiresAuthorityToDisarm = false;
    disarmTimerStarted = false;
    stopSiren = true;
    triggeringAlarmTimer.stopTimer();
    settingAlarmTimer.stopTimer();
    reentryTimer.stopTimer();
    disarmTimer.stopTimer();
    codeEntryTimer.stopTimer();
    sirenDurationTimer.stopTimer();

    for (auto &sensor: sensors) {
        sensor->reset();
    }
}

/*
 * Function to run waitTimer method of each timer in a separate thread to update the timer information
 */
void AlarmControlSystem::startTimerThreads() {
    settingAlarmThread = thread([this]() { this->settingAlarmTimer.waitTimer(); });
    triggeringAlarmThread = thread([this]() { this->triggeringAlarmTimer.waitTimer(); });
    reentryTimerThread = thread([this]() { this->reentryTimer.waitTimer(); });
    disarmTimerThread = thread([this]() { this->disarmTimer.waitTimer(); });
    codeEntryTimerThread = thread([this]() { this->codeEntryTimer.waitTimer(); });
    sirenDurationTimerThread = thread([this]() { this->sirenDurationTimer.waitTimer(); });
}

/*
 * Function to stop all the threads
 */
void AlarmControlSystem::joinTimerThreads() {
    if (settingAlarmThread.joinable()) settingAlarmThread.join();
    if (triggeringAlarmThread.joinable()) triggeringAlarmThread.join();
    if (reentryTimerThread.joinable()) reentryTimerThread.join();
    if (disarmTimerThread.joinable()) disarmTimerThread.join();
    if (codeEntryTimerThread.joinable()) codeEntryTimerThread.join();
    if (sirenDurationTimerThread.joinable()) sirenDurationTimerThread.join();
}

/*
 * Function to allow an authority to disarm the system
 * As there was no specification on how the authority should disarm the system,
 * the system can be disarmed by simply calling this function when the system needs an independent authority
 * is needed to disarm the system
 */
void AlarmControlSystem::authorityDisarmSystem() {
    if (requiresAuthorityToDisarm) {
        isArmed = false;
        requiresAuthorityToDisarm = false;
        triggeringAlarmTimer.stopTimer();
        settingAlarmTimer.stopTimer();
        reentryTimer.stopTimer();
        disarmTimer.stopTimer();
        codeEntryTimer.stopTimer();
        sirenDurationTimer.stopTimer();
        cout << "Alarm disarmed by authority." << endl;
        turnOffAlarm();
        alarmTriggered = false;
    }
}
