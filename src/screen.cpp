#include <M5StickC.h>
#include "screen.h"
#include "dispatch.h"
#include "pulse.h"

void screenTask(void * pvParameter) {
    dispatch_pair_t core = *((dispatch_pair_t *) pvParameter);
    char buf[40];
    M5.Lcd.begin();
    M5.Lcd.setRotation(1);
    while (1) {
        dispatch_cmd_t curcmd;
        if (xQueueReceive(core.cmdq, &curcmd, portMAX_DELAY) == pdTRUE) {
            if (curcmd.type == SCREEN_SHOW_HOME) {
                M5.Lcd.setTextFont(4);
                snprintf(buf, sizeof(buf), "RPM: %-5i     ", getRPM());
                M5.Lcd.drawString(buf, 0, 0);
                snprintf(buf, sizeof(buf), "PWR: %-5i     ", curcmd.data);
                M5.Lcd.drawString(buf, 0, 40);                
            }
        }
    }

}