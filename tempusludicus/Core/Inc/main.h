#ifndef MAIN_H
#define MAIN_H

#include "common.h"
#include "enumerations.h"

typedef struct sytem_state_s {
    enum e_switchState switchState;
    enum e_programState programState;
    enum e_mood mood;
    enum e_developer person;
} system_state_t;

extern volatile system_state_t system_state;

void HardFault_Handler(void);

#endif // MAIN_H
