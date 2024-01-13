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
    if (PIN_SW1_PORT->ISFR & PIN_SW1) {
        if (PIN_SW1_PT->PDIR & PIN_SW1) {
            buttonState_1 = 1;
        }

        // Clear the flag
        PIN_SW1_PORT->ISFR = PIN_SW1;
    }

    // switch 2
    if (PIN_SW2_PORT->ISFR & PIN_SW2) {
        if (PIN_SW2_PT->PDIR & PIN_SW2) {
            buttonState_2 = 1;
        }

        // Clear the flag
        PIN_SW2_PORT->ISFR = PIN_SW2;
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
