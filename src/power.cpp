#include <M5StickC.h>

#include "dispatch.h"
#include "power.h"

#define POWER_CHECK_INTERVAL_MS 100
#define POWER_NOTIFY_AFTER_CHECKS_NUM 10

void powerTask(void * pvParameter) {
    dispatch_pair_t core = *((dispatch_pair_t *) pvParameter);
    int poweroffchecks = 0;
    while (1) {
        if (M5.Axp.GetVinVoltage() < 3) {
            poweroffchecks += 1;
        } 
        else {
            poweroffchecks = 0;
        }
        if (poweroffchecks >= POWER_NOTIFY_AFTER_CHECKS_NUM) {
            dispatch_evt_t outevt;
            outevt.type = GUST_EVT_POWER_DISCONNECTED;
            xQueueSend(core.evtq, &outevt, portMAX_DELAY);
        }
        vTaskDelay(POWER_CHECK_INTERVAL_MS/portTICK_PERIOD_MS);
    }
}