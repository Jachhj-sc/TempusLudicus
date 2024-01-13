#ifndef MAIN_H
#define MAIN_H

#include "common.h"

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
    PROGRAMSTATE_AMOUNT
};

enum e_mood {
    DEFAULT_MOOD,
    COOL,
    WARM,
    EXCITED,
    MELLOW,
    CHILL,
    MOOD_AMOUNT
};

enum e_time_effect {
    STATIC,
    RAIN,
    RAINBOW_DISTINGUISED,
    HUE,
	TIME_EFFECT_AMOUNT
};

enum e_developer {
    Kevin,
    Roel,
    Maarten,
    William,
	DEVELOPER_AMOUNT
};

typedef struct sytem_state_s {
	enum e_switchState switchState;
	enum e_programState programState;
    enum e_mood mood;
    enum e_time_effect effect;
    enum e_developer person;
} system_state_t;

extern volatile system_state_t system_state;

void HardFault_Handler(void);

#endif
