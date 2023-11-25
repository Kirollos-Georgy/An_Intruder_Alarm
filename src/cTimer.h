#ifndef AN_INTRUDER_ALARM_CTIMER_H
#define AN_INTRUDER_ALARM_CTIMER_H

#include <iostream>
#include <chrono>
#include <thread>
#include <sys/neutrino.h>
#include <sys/siginfo.h>
#include <sys/time.h>
#include <unistd.h>

class cTimer {
private:
    int channelId;
    int connectionId;

    struct sigevent sigEvent;
    struct timerspec timerSpec;
    timer_t timerId;

    char msgBuffer[100];

    uint64_t cyclesPerSec;
    uint64_t tickCycles;
    uint64_t tookCycles;

public:
    cTimer(uint32_t,uint32_t);

    void setTimerSpec(uint32_t,uint32_t);
    void waitTimer();
    void startTimer();
    void stopTimer();
    void startSingleShotTimer(uint32_t, uint32_t);
    virtual ~cTimer();
};

#endif //AN_INTRUDER_ALARM_CTIMER_H
