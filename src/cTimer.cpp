#include "cTimer.h"

/*
 * Initialising the timer
 */
cTimer::cTimer(uint32_t sec, uint32_t msec) {
    initial_sec = sec;
    initial_nsec = msec * 1000000;
    channelId = ChannelCreate(0);
    connectionId = ConnectAttach(0, 0, channelId, 0, 0);
    if (connectionId == -1) {
        std::cerr << "Timer, Connect Attach error : " << errno << "\n";
    }

    SIGEV_PULSE_INIT(&sigEvent, connectionId, SIGEV_PULSE_PRIO_INHERIT, 1, 0);

    //Create timer
    if (timer_create(CLOCK_REALTIME, &sigEvent, &timerId) == -1) {
        std::cerr << "Timer, Init error : " << errno << "\n";
    }

    setTimerSpec(sec, 1000000 * msec);

    cyclesPerSec = SYSPAGE_ENTRY(qtime)->cycles_per_sec;

    isTimerRunning = false;
    isTimerExpired = false;
}

cTimer::~cTimer() {
    timer_delete(timerId);
    ConnectDetach(connectionId);
    ChannelDestroy(channelId);
}

void cTimer::setTimerSpec(uint32_t sec, uint32_t nano) {
    initial_sec = sec;
    initial_nsec = nano;
    timerSpec.it_value.tv_sec = sec;
    timerSpec.it_value.tv_nsec = nano;
    timerSpec.it_interval.tv_sec = sec;
    timerSpec.it_interval.tv_nsec = nano;

    //Start timer
    timer_settime(timerId, 0, &timerSpec, NULL);
}

/*
 * Starting the timer
 */
void cTimer::startTimer() {
	isTimerRunning = true;
	isTimerExpired = false;
	timer_settime(timerId, 0, &timerSpec, NULL);
}

/*
 * Stopping the timer
 */
void cTimer::stopTimer() {
	isTimerRunning = false;
    timerSpec.it_value.tv_sec = 0;
    timerSpec.it_value.tv_nsec = 0;
    timerSpec.it_interval.tv_sec = 0;
    timerSpec.it_interval.tv_nsec = 0;

    timer_settime(timerId, 0, &timerSpec, NULL);
}

//Waiting on the timer to update the timer flags
void cTimer::waitTimer() {
	struct _pulse pulse;
    int rcvid;
    rcvid = MsgReceive(channelId, &pulse, sizeof(pulse), NULL);

    if (rcvid == 0 && pulse.code == 1) {
    	isTimerExpired = true;
        isTimerRunning = false;
    }
}

/*
 * Starting the timer in a single shot
 */
void cTimer::startSingleShotTimer(uint32_t sec, uint32_t nsec) {
	isTimerRunning = true;
	isTimerExpired = false;
    timerSpec.it_value.tv_sec = sec;
    timerSpec.it_value.tv_nsec = nsec;
    timerSpec.it_interval.tv_sec = 0;
    timerSpec.it_interval.tv_nsec = 0;

    if (timer_settime(timerId, 0, &timerSpec, NULL) == -1) {
        std::cerr << "Timer, Set time error: " << strerror(errno) << std::endl;
    }
}

/*
 * Checking if the timer has expired
 */
bool cTimer::isExpired() {
    if (isTimerExpired) {
        isTimerExpired = false;
        return true;
    }
    return false;
}

/*
 * Checking if the timer is running
 */
bool cTimer::isRunning() {
    return isTimerRunning;
}
