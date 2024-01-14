#ifndef ENUMERATIONS_H
#define ENUMERATIONS_H

enum e_switchState {
    NO_SWITCH_PRESSED,
    SWITCH_1_PRESSED,
    SWITCH_2_PRESSED,
    SWITCH_1_2_PRESSED
};

enum e_programState {
    DRAWSTRIP = 0,
    ULTRASOON,
    PENSIOEN,
    DEBUG,
    TEMPSENSOR,
    PROGRAMSTATE_AMOUNT
};

extern const char *e_state_name[PROGRAMSTATE_AMOUNT];

enum e_mood {
    DEFAULT_MOOD,
    COOL,
    WARM,
    EXCITED,
    MELLOW,
    CHILL,
    RAIN,
    RAINBOW,
    HUE_GRADIENT,
    MOOD_AMOUNT
};

extern const char *e_mood_name[MOOD_AMOUNT];

enum e_developer {
    Kevin,
    Roel,
    Maarten,
    William,
    DEVELOPER_AMOUNT
};

#endif // ENUMERATIONS_H
