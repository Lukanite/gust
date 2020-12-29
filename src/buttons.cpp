#include <M5StickC.h>
#include "buttons.h"
#include "dispatch.h"

static const char * TAG = "BTN";

#define BUTTON_DEBOUNCE_US 500000

#define BUTTON_SPEED_PIN G37

#define BUTTON_SPEED_BIT BIT0
static EventGroupHandle_t buttonEvtBits;
static int64_t lastButtonSpeedPressedTime;

static void IRAM_ATTR buttonSpeedPressed() {
    BaseType_t xHigherPriorityTaskWoken, xResult;
    xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBitsFromISR(buttonEvtBits, BUTTON_SPEED_BIT, &xHigherPriorityTaskWoken);
    if( xResult != pdFAIL )
    {
        /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
        switch should be requested.  The macro used is port specific and will
        be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
        the documentation page for the port being used. */
        portYIELD_FROM_ISR();
    }
}

void buttonTask(void * pvParameter) {
    dispatch_pair_t core = *((dispatch_pair_t *) pvParameter);
    buttonEvtBits = xEventGroupCreate();
    lastButtonSpeedPressedTime = 0;
    pinMode(BUTTON_SPEED_PIN, INPUT_PULLUP);
    attachInterrupt(BUTTON_SPEED_PIN, buttonSpeedPressed, FALLING);
    while (1) {
        EventBits_t bits = xEventGroupWaitBits(buttonEvtBits, BUTTON_SPEED_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
        if (bits & BUTTON_SPEED_BIT) {
            int64_t curtime = esp_timer_get_time();
            dispatch_evt_t evt = {
                .type = GUST_EVT_BUTTON_SPEED_PRESSED
            };
            if (curtime - lastButtonSpeedPressedTime > BUTTON_DEBOUNCE_US) {
                xQueueSend(core.evtq, &evt, portMAX_DELAY);
                lastButtonSpeedPressedTime = curtime;
            }
            
        }
    }
}
