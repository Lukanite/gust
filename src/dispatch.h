#ifndef M5F_DISPATCH_H
#define M5F_DISPATCH_H

#include <M5StickC.h>
#include "commands.h"
#include "events.h"

typedef struct dispatch_pair {
    QueueHandle_t evtq;
    QueueHandle_t cmdq;
} dispatch_pair_t;

#endif