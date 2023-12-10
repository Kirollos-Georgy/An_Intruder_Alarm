#ifndef AN_INTRUDER_ALARM_CTIMER_H
#define AN_INTRUDER_ALARM_CTIMER_H

#include <iostream>
#include <chrono>
#include <thread>
#include <sys/neutrino.h>
#include <sys/siginfo.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <sync.h>
#include <sys/netmgr.h>
#include <sys/syspage.h>
#include <inttypes.h>
#include <stdint.h>

class cTimer {
private:
    int channelId;
    int connectionId;

    struct sigevent sigEvent;
    struct itimerspec timerSpec;
    timer_t timerId;

    char msgBuffer[100];

    uint64_t cyclesPerSec;
    uint64_t tickCycles;
    uint64_t tookCycles;

    uint32_t initial_sec;
    uint32_t initial_nsec;

    bool isTimerRunning;
    bool isTimerExpired;

public:
    cTimer(uint32_t,uint32_t);

    void setTimerSpec(uint32_t,uint32_t);
    void waitTimer();
    void startTimer();
    void stopTimer();
    void startSingleShotTimer(uint32_t, uint32_t);
    void restartSingleShorTImer();

    bool isExpired();
    bool isRunning();

    std::thread timerThread;
    void timerThreadFunction();

    virtual ~cTimer();
};

#endif //AN_INTRUDER_ALARM_CTIMER_H
