#include "cTimer.h"

cTimer::cTimer(uint32_t sec, uint32_t msec) {
    channel_id = ChannelCreate(0);
    connection_id = ConnectAttach(0, 0, channel_id, 0, 0);
    if (connection_id == -1) {
        std::cerr << "Timer, Connect Attach error : " << errno << "\n";
    }

    SIGEV_PULSE_INIT(&sig_event, connection_id, SIGEV_PULSE_PRIO_INHERIT, 1, 0);

    //Create timer
    if (timer_create(CLOCK_REALTIME, &sig_event, &timer_id) == -1) {
        std::cerr << "Timer, Init error : " << errno << "\n";
    }

    setTimerSpec(sec, 1000000 * msec);

    cycles_per_sec = SYSPAGE_ENTRY(qtime)->cycles_per_sec;
}

cTimer::~cTimer() {
    timer_delete(timer_id);
    ConnectDetach(connection_id);
    ChannelDestroy(channel_id);
}

void cTimer::setTimerSpec(uint32_t sec, uint32_t nano) {
    timer_spec.it_value.tv_sec = sec;
    timer_spec.it_value.tv_nsec = nano;
    timer_spec.it_interval.tv_sec = sec;
    timer_spec.it_interval.tv_nsec = nano;

    //Start timer
    timer_settime(timer_id, 0, &timer_spec, NULL);
}

void cTimer::startTimer() {
    timer_settime(timer_id, 0, &timer_spec, NULL);
}

void cTimer::stopTimer() {
    // Set the timer_spec values to 0 to stop the timer
    timer_spec.it_value.tv_sec = 0;
    timer_spec.it_value.tv_nsec = 0;
    timer_spec.it_interval.tv_sec = 0;
    timer_spec.it_interval.tv_nsec = 0;

    timer_settime(timer_id, 0, &timer_spec, NULL)
}

void cTimer::waitTimer() {
    int rcvid;
    rcvid = MsgReceive(channel_id, &msg_buffer, sizeof(msg_buffer), NULL);
}

void cTimer::startSingleShotTimer(uint32_t sec, uint32_t nsec) {
    timer_spec.it_value.tv_sec = sec;
    timer_spec.it_value.tv_nsec = nsec;
    timer_spec.it_interval.tv_sec = 0; // Zero means it's a single-shot timer
    timer_spec.it_interval.tv_nsec = 0;

    if (timer_settime(timer_id, 0, &timer_spec, NULL) == -1) {
        std::cerr << "Timer, Set time error: " << strerror(errno) << std::endl;
        // Consider throwing an exception or returning an error code here
    }
}