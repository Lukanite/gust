#ifndef M5F_COMMANDS_H
#define M5F_COMMANDS_H

typedef enum dispatch_cmd_type {
    LEDS_SHOW_RAINBOW,
    FAN_SET_PERCENT,
    SCREEN_SHOW_HOME
} dispatch_cmd_type_t;

typedef struct dispatch_cmd {
    dispatch_cmd_type_t type;
    int data;
    void * extdata;
} dispatch_cmd_t;

#endif