#ifndef M5F_PULSE_H
#define M5F_PULSE_H

#include <M5StickC.h>

#define PULSE_GPIO G36
#define PULSE_UNIT PCNT_UNIT_0
#define PULSE_ZERO_THRESHOLD 1000000

typedef struct pulse_info {
    int64_t lastPulseTime; //Time of latest measurement
    int64_t earlierPulseTime; //Time of previous from latest measurement
    int16_t lastPulseCount;
    uint16_t debugNewPulses;
    uint64_t debugDuration;
} pulse_info_t;

extern volatile pulse_info_t PulseInfo; //For debugging

void initializePulseCounter();
int16_t getRPM();

#endif