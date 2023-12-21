#ifndef MAIN_H
#define MAIN_H

#include "common.h"

#define BUTTON_INTERVAL 500u // ms until a new buttonpress can be registered

enum e_switchState {
    NO_SWITCH_PRESSED,
    SWITCH_1_PRESSED,
    SWITCH_2_PRESSED,
    SWITCH_1_2_PRESSED
};

enum e_programState {
    DRAWSTRIP = 0,
    TIMELCD,
    ULTRASOON,
    PENSIOEN,
    DEBUG,
    TEMPSENSOR,
    StateAmount,
    TESTSEQUENCE
};

void HardFault_Handler(void);

#endif
