#include <M5StickC.h>
#include "screen.h"
#include "dispatch.h"
#include "pulse.h"

void screenTask(void * pvParameter) {
    dispatch_pair_t core = *((dispatch_pair_t *) pvParameter);
    M5.Lcd.begin();
    M5.Lcd.setRotation(1);
    while (1) {
        dispatch_cmd_t curcmd;
        if (xQueueReceive(core.cmdq, &curcmd, portMAX_DELAY) == pdTRUE) {
            if (curcmd.type == SCREEN_SHOW_HOME) {
                M5.Lcd.setCursor(0,0);
                M5.Lcd.fillScreen(TFT_BLACK);
                M5.Lcd.setTextFont(2);
                M5.Lcd.drawString("RPM:", 0, 0);
                M5.Lcd.drawNumber(getRPM(), 40, 0);
                M5.Lcd.drawString("New:", 0, 20);
                M5.Lcd.drawNumber(PulseInfo.debugNewPulses, 40, 20);
                M5.Lcd.drawString("Dur: ", 0, 40);
                M5.Lcd.drawNumber(PulseInfo.debugDuration, 40, 40);
            }
        }
    }

}