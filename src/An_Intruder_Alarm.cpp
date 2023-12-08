#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include "AlarmControlSystem.h"

using namespace std;

void sensorCheckLoop(AlarmControlSystem& alarmSystem) {
    while (true) {
        alarmSystem.checkSensors();
        this_thread::sleep_for(chrono::seconds(1)); // Check sensors every second
    }
}

int main() {
    AlarmControlSystem alarmSystem;

    Sensor* entry1 = new Sensor("entry1");
    Sensor* nominatedExit = new Sensor("nominatedExit");

    alarmSystem.addSensor(entry1);
    alarmSystem.addSensor(nominatedExit);

    thread sensorCheckThread(sensorCheckLoop, ref(alarmSystem));
    alarmSystem.startTimerThreads();

    auto runScenario = [&](std::function<void()> scenario) {
    	alarmSystem.resetSystem();
        entry1->reset();
        nominatedExit->reset();
        scenario();
        this_thread::sleep_for(chrono::seconds(5)); // Wait for scenario to process
        cout << endl;
    };

    /*runScenario([&] {
    	cout << "Scenario 1: Arming and leaving the building within 30 seconds" << endl;
    	alarmSystem.armSystem("1234");
    	this_thread::sleep_for(chrono::seconds(20));
    	nominatedExit->trigger();
    	this_thread::sleep_for(chrono::seconds(1));
    	nominatedExit->reset();
    });*/

    /*runScenario([&] {
        cout << "Scenario 2: Arming and leaving the building after 30 seconds" << endl;
        alarmSystem.armSystem("1234");
        this_thread::sleep_for(chrono::seconds(35));
        nominatedExit->trigger();
        this_thread::sleep_for(chrono::seconds(1));
        nominatedExit->reset();
    });*/

    /*runScenario([&] {
       	cout << "Scenario 3: Arming and leaving the building not through the nominated exit" << endl;
       	alarmSystem.armSystem("1234");
       	this_thread::sleep_for(chrono::seconds(20));
       	entry1->trigger();
       	this_thread::sleep_for(chrono::seconds(1));
       	entry1->reset();
     });*/

    /*runScenario([&] {
        cout << "Scenario 4: Triggering a sensor after arming the system" << endl;
        //Arming the system and leaving in time
        alarmSystem.armSystem("1234");
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
    });*/

    /*runScenario([&] {
        cout << "Scenario 5: Re-entering and disarming" << endl;
        //Arming the system and leaving in time
        alarmSystem.armSystem("1234");
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
        alarmSystem.disarmSystem("1234");
    });*/

    /*runScenario([&] {
    	cout << "Scenario 6: Re-entering the building but not re-entring within 2 minutes" << endl;
        //Arming the system and leaving in time
        alarmSystem.armSystem("1234");
        this_thread::sleep_for(chrono::seconds(20));
        nominatedExit->trigger();
        this_thread::sleep_for(chrono::seconds(1));
        nominatedExit->reset();

        //Waiting a while before entering again
        this_thread::sleep_for(chrono::seconds(10));

        nominatedExit->trigger();
        this_thread::sleep_for(chrono::seconds(1));
        nominatedExit->reset();
        this_thread::sleep_for(chrono::seconds(130));
    });*/

    runScenario([&] {
    	cout << "Scenario 7: Re-entering the building but not entering the code within 1 minute" << endl;
        //Arming the system and leaving in time
        alarmSystem.armSystem("1234");
        this_thread::sleep_for(chrono::seconds(20));
        nominatedExit->trigger();
        this_thread::sleep_for(chrono::seconds(1));
        nominatedExit->reset();

        //Waiting a while before entering again
        this_thread::sleep_for(chrono::seconds(10));

        nominatedExit->trigger();
        this_thread::sleep_for(chrono::seconds(1));
        nominatedExit->reset();
        this_thread::sleep_for(chrono::seconds(60));

        alarmSystem.disarmSystem("1234");
        this_thread::sleep_for(chrono::seconds(70));
    });

    runScenario([&] {
        cout << "Scenario 5: Failing to disarm in time" << endl;
        alarmSystem.armSystem("1234");
        nominatedExit->trigger();
        this_thread::sleep_for(chrono::seconds(120));
    });

    runScenario([&] {
        cout << "Scenario 6: Entering incorrect disarm code" << endl;
        alarmSystem.disarmSystem("0000");
    });

    sensorCheckThread.detach();
    alarmSystem.joinTimerThreads();

    return 0;
}
