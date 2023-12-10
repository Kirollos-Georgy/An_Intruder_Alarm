#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include "AlarmControlSystem.h"

using namespace std;

/*
 * Function to keep check the sensors status (triggered or not) every 1 second
 */
void sensorCheckLoop(AlarmControlSystem &alarmSystem) {
    while (true) {
        alarmSystem.checkSensors();
        this_thread::sleep_for(chrono::seconds(1)); // Check sensors every second
    }
}

int main() {

    // Creating an AlarmControlSystem instance
    AlarmControlSystem alarmSystem;

    //Creating two sensors
    Sensor *entry1 = new Sensor("entry1");
    Sensor *nominatedExit = new Sensor("nominatedExit");

    //Adding the sensors to the AlarmControlSystem instance
    alarmSystem.addSensor(entry1);
    alarmSystem.addSensor(nominatedExit);

    //Creating a thread to check the sensors status every second
    thread sensorCheckThread(sensorCheckLoop, ref(alarmSystem));

    //Creating timer threads to keep updating each timer status
    alarmSystem.startTimerThreads();

    auto runScenario = [&](std::function<void()> scenario) {
        //Reset the instance before running each scenario
        alarmSystem.resetSystem();

        //Run the specified scenario
        scenario();

        // Wait for scenario to process
        this_thread::sleep_for(chrono::seconds(5));
        cout << endl;
    };

    cout << "Starting the test cases" << endl;
    cout << endl;

    //For the following part, there are 9 scenarios to fully test the system
    //The tests are commented as running all the tests will take about 20 minutes
    //therefore, you can un-comment the test you need to run and run the program to save time
    //of waiting on all the tests to finish to reach the desired test
    //You will know when the tests finish by seeing the message "Tests have ended"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //Scenario 1: Arming and leaving the building within 30 seconds
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*    runScenario([&] {
    	cout << "Scenario 1: Arming and leaving the building within 30 seconds" << endl;
    	//Arming the system
        alarmSystem.armSystem('1');
        alarmSystem.armSystem('2');
        alarmSystem.armSystem('3');
        alarmSystem.armSystem('4');

        //Leaving the building in time
    	this_thread::sleep_for(chrono::seconds(20));
    	//Triggering the sensor to simulate leaving the building
    	nominatedExit->trigger();
    	this_thread::sleep_for(chrono::seconds(1));
    	nominatedExit->reset();
    });*/

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //Scenario 2: Arming the system but entering the code in more than 20 seconds
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*    runScenario([&] {
    	cout << "Scenario 2: Arming the system but entering the code in more than 20 seconds" << endl;
    	//Arming the system
        alarmSystem.armSystem('1');
        //Taking more than 20 seconds to enter the code
        this_thread::sleep_for(chrono::seconds(20));
        alarmSystem.armSystem('2');

    	//Arming the system
        alarmSystem.armSystem('1');
        alarmSystem.armSystem('2');
        alarmSystem.armSystem('3');
        alarmSystem.armSystem('4');

        //Leaving the building in time
        this_thread::sleep_for(chrono::seconds(20));
        //Triggering the sensor to simulate leaving the building
        nominatedExit->trigger();
        this_thread::sleep_for(chrono::seconds(1));
        nominatedExit->reset();
    });*/

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //Scenario 3: Arming and leaving the building after 30 seconds
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*    runScenario([&] {
        cout << "Scenario 3: Arming and leaving the building after 30 seconds" << endl;
        //Arming the system
        alarmSystem.armSystem('1');
        alarmSystem.armSystem('2');
        alarmSystem.armSystem('3');
        alarmSystem.armSystem('4');

        //Taking more than 30 seconds to leave the building
        this_thread::sleep_for(chrono::seconds(35));
        //Triggering the sensor to simulate leaving the building
        nominatedExit->trigger();
        this_thread::sleep_for(chrono::seconds(1));
        nominatedExit->reset();
    });*/

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //Scenario 4: Arming and leaving the building not through the nominated exit
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*    runScenario([&] {
       	cout << "Scenario 4: Arming and leaving the building not through the nominated exit" << endl;
       	//Arming the system
        alarmSystem.armSystem('1');
        alarmSystem.armSystem('2');
        alarmSystem.armSystem('3');
        alarmSystem.armSystem('4');

        //Leaving the building in time
       	this_thread::sleep_for(chrono::seconds(20));
       	//Triggering the sensor to simulate leaving the building
       	entry1->trigger();
       	this_thread::sleep_for(chrono::seconds(1));
       	entry1->reset();
     });*/

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //Scenario 5: Triggering a sensor after arming the system
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*    runScenario([&] {
        cout << "Scenario 5: Triggering a sensor after arming the system" << endl;
        //Arming the system and leaving in time
        alarmSystem.armSystem('1');
        alarmSystem.armSystem('2');
        alarmSystem.armSystem('3');
        alarmSystem.armSystem('4');
        this_thread::sleep_for(chrono::seconds(20));
        nominatedExit->trigger();
        this_thread::sleep_for(chrono::seconds(1));
        nominatedExit->reset();

        //Waiting a while to then trigger the sensor
        this_thread::sleep_for(chrono::seconds(10));

        //Triggering the sensor
        entry1->trigger();
        this_thread::sleep_for(chrono::seconds(1));
        entry1->reset();

        this_thread::sleep_for(chrono::seconds(10));
    });*/

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //Scenario 6: Re-entering and disarming
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*    runScenario([&] {
        cout << "Scenario 6: Re-entering and disarming" << endl;
        //Arming the system and leaving in time
        alarmSystem.armSystem('1');
        alarmSystem.armSystem('2');
        alarmSystem.armSystem('3');
        alarmSystem.armSystem('4');

        //Leaving the building in time
        this_thread::sleep_for(chrono::seconds(20));

        //Triggering the sensor to simulate leaving the building
        nominatedExit->trigger();
        this_thread::sleep_for(chrono::seconds(1));
        nominatedExit->reset();

        //Waiting a while before entering again
        this_thread::sleep_for(chrono::seconds(10));

		//Triggering the sensor to simulate entering the building
        nominatedExit->trigger();
        this_thread::sleep_for(chrono::seconds(1));
        nominatedExit->reset();
        this_thread::sleep_for(chrono::seconds(30));

        //Disarming the system
        alarmSystem.disarmSystem('1');
        alarmSystem.disarmSystem('2');
        alarmSystem.disarmSystem('3');
        alarmSystem.disarmSystem('4');
    });*/

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //Scenario 7: Re-entering the building but not re-entring within 2 minutes
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*    runScenario([&] {
    	cout << "Scenario 7: Re-entering the building but not re-entring within 2 minutes" << endl;
        //Arming the system and leaving in time
        alarmSystem.armSystem('1');
        alarmSystem.armSystem('2');
        alarmSystem.armSystem('3');
        alarmSystem.armSystem('4');
        this_thread::sleep_for(chrono::seconds(20));
        nominatedExit->trigger();
        this_thread::sleep_for(chrono::seconds(1));
        nominatedExit->reset();

        //Waiting a while before entering again
        this_thread::sleep_for(chrono::seconds(10));

		//Triggering the sensor to simulate entering the building
        nominatedExit->trigger();
        this_thread::sleep_for(chrono::seconds(1));
        nominatedExit->reset();

        this_thread::sleep_for(chrono::seconds(130));
    });*/

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //Scenario 8: Re-entering the building but not entering the correct code within 1 minute
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*    runScenario([&] {
    	cout << "Scenario 8: Re-entering the building but not entering the correct code within 1 minute" << endl;
        //Arming the system and leaving in time
        alarmSystem.armSystem('1');
        alarmSystem.armSystem('2');
        alarmSystem.armSystem('3');
        alarmSystem.armSystem('4');
        this_thread::sleep_for(chrono::seconds(20));
        nominatedExit->trigger();
        this_thread::sleep_for(chrono::seconds(1));
        nominatedExit->reset();

        //Waiting a while before entering again
        this_thread::sleep_for(chrono::seconds(10));

        nominatedExit->trigger();
        this_thread::sleep_for(chrono::seconds(1));
        nominatedExit->reset();
        this_thread::sleep_for(chrono::seconds(30));

		//Entering the wrong code
        alarmSystem.disarmSystem('1');
        alarmSystem.disarmSystem('2');
        alarmSystem.disarmSystem('3');
        alarmSystem.disarmSystem('3');

        this_thread::sleep_for(chrono::seconds(60));

        //Entering the correct code
        alarmSystem.disarmSystem('4');
        alarmSystem.disarmSystem('1');
        alarmSystem.disarmSystem('2');
        alarmSystem.disarmSystem('3');
        alarmSystem.disarmSystem('4');

        alarmSystem.authorityDisarmSystem();

        this_thread::sleep_for(chrono::seconds(10));
    });*/

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //Scenario 9: Triggering the alarm and leaving it triggered for 5 minutes and then disarming it
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*    runScenario([&] {
        cout << "Scenario 9: Triggering the alarm and leaving it triggered for 5 minutes and then disarming it" << endl;
        alarmSystem.armSystem('1');
        alarmSystem.armSystem('2');
        alarmSystem.armSystem('3');
        alarmSystem.armSystem('4');
        this_thread::sleep_for(chrono::seconds(20));
        nominatedExit->trigger();
        this_thread::sleep_for(chrono::seconds(1));
        nominatedExit->reset();

        this_thread::sleep_for(chrono::seconds(10));

        entry1->trigger();
        this_thread::sleep_for(chrono::seconds(1));
        entry1->reset();

        this_thread::sleep_for(chrono::seconds(6 * 60));

        alarmSystem.disarmSystem('1');
        alarmSystem.disarmSystem('2');
        alarmSystem.disarmSystem('3');
        alarmSystem.disarmSystem('4');

        this_thread::sleep_for(chrono::seconds(10));

    });*/

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //Scenario 10: Triggering a sensor after disarming the system
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*    runScenario([&] {
        cout << "Scenario 10: Triggering a sensor after disarming the system" << endl;
        //Arming the system and leaving in time
        alarmSystem.armSystem('1');
        alarmSystem.armSystem('2');
        alarmSystem.armSystem('3');
        alarmSystem.armSystem('4');
        this_thread::sleep_for(chrono::seconds(20));
        nominatedExit->trigger();
        this_thread::sleep_for(chrono::seconds(1));
        nominatedExit->reset();

        //Waiting a while before entering again
        this_thread::sleep_for(chrono::seconds(10));

        nominatedExit->trigger();
        this_thread::sleep_for(chrono::seconds(1));
        nominatedExit->reset();
        this_thread::sleep_for(chrono::seconds(30));
        alarmSystem.disarmSystem('1');
        alarmSystem.disarmSystem('2');
        alarmSystem.disarmSystem('3');
        alarmSystem.disarmSystem('4');

        entry1->trigger();
        this_thread::sleep_for(chrono::seconds(1));
        entry1->reset();

        this_thread::sleep_for(chrono::seconds(10));
    });*/

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //Scenario 11: Triggering the alarm and then disarming it
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*    runScenario([&] {
        cout << "Scenario 11: Triggering the alarm and then disarming it" << endl;
        alarmSystem.armSystem('1');
        alarmSystem.armSystem('2');
        alarmSystem.armSystem('3');
        alarmSystem.armSystem('4');
        this_thread::sleep_for(chrono::seconds(20));
        nominatedExit->trigger();
        this_thread::sleep_for(chrono::seconds(1));
        nominatedExit->reset();

        this_thread::sleep_for(chrono::seconds(10));

        entry1->trigger();
        this_thread::sleep_for(chrono::seconds(1));
        entry1->reset();

        this_thread::sleep_for(chrono::seconds(40));
        alarmSystem.disarmSystem('1');
        alarmSystem.disarmSystem('2');
        alarmSystem.disarmSystem('3');
        alarmSystem.disarmSystem('4');

        this_thread::sleep_for(chrono::seconds(10));

    });*/

    cout << "Tests have ended" << endl;

    //Handling the threads
    sensorCheckThread.detach();
    alarmSystem.joinTimerThreads();

    return 0;
}
