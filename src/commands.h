#ifndef M5F_COMMANDS_H
#define M5F_COMMANDS_H

typedef enum dispatch_cmd_type {
    GUST_CMD_LEDS_SHOW_RAINBOW,
    GUST_CMD_FAN_SET_PERCENT,
    GUST_CMD_SCREEN_SHOW_HOME,
    GUST_CMD_SCREEN_UPDATE_WIFI_IP,
    GUST_CMD_SCREEN_SHOW_POWEROFF
} dispatch_cmd_type_t;

typedef struct dispatch_cmd {
    dispatch_cmd_type_t type;
    int32_t data;
    void * extdata;
} dispatch_cmd_t;

#endif