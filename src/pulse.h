#ifndef M5F_PULSE_H
#define M5F_PULSE_H

#include <M5StickC.h>

#define PULSE_GPIO G36
#define PULSE_ZERO_THRESHOLD 1000000 //If no pulses more recent than this (us), 0 rpm

typedef struct pulse_info {
    int64_t latestPulseTime; //Time of last pulse
    int64_t latestPulseDuration; //Last measured duration
} pulse_info_t;

extern pulse_info_t PulseInfo; //For debugging

void initializePulseCounter();
int16_t getRPM();

#endif