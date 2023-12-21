/**
    @file it_handlers.c
    @author William Hak (william.hak57@gmail.com)
    @brief place for all the used intterupt handler callback functions
    @version 0.1
    @date 2023-12-13

    @copyright Copyright (c) 2023

 */
#include "it_handlers.h"

#include "switches.h"
#include "ultrasonic_sensor.h"

void PORTD_IRQHandler(void)
{
    // switch 1
    if ((PORTD->ISFR & (1 << 0))) {
        if (PTD->PDIR & MASK(0)) {
            buttonState_1 = 1;
        }

        // Clear the flag
        PORTD->ISFR = (1 << 0);
    }

    // switch 2
    if ((PORTD->ISFR & (1 << 3))) {
        if (PTD->PDIR & MASK(3)) {
            buttonState_2 = 1;
        }

        // Clear the flag
        PORTD->ISFR = (1 << 3);
    }

    // pulse from ultrasoon sensor interrupt
    if ((PORTD->ISFR & (1 << 5))) {
        if (PTD->PDIR & MASK(5)) {
            tpm1_start();
        }
        if (!(PTD->PDIR & MASK(5))) {
            ultraS_updateDistance(tpm1_stop());
            tpm1_reset();
        }
        // Clear the flag
        PORTD->ISFR = (1 << 5);
    }
}
