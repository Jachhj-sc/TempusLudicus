/**
    @file rgb.c
    @author William Hak (william.hak57@gmail.com)
    @brief Implementation of rgb led on the mlk25z
    @version 0.1
    @date 2023-09-28

    @copyright Copyright (c) 2023

 */
#include "rgb.h"
#include "main.h"

void init_rgb(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; // enable clock
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;

    // set pin mux to GPIO
    RED_LED_PORT->PCR[RED_LED_SHIFT] = PORT_PCR_MUX(1);
    GREEN_LED_PORT->PCR[GREEN_LED_SHIFT] = PORT_PCR_MUX(1);
    BLUE_LED_PORT->PCR[BLUE_LED_SHIFT] = PORT_PCR_MUX(1);

    // set pins to output
    RED_LED->PDDR |= RED_LED_MASK;
    GREEN_LED->PDDR |= GREEN_LED_MASK;
    BLUE_LED->PDDR |= BLUE_LED_MASK;

    // turn leds off
    RED_LED->PSOR |= RED_LED_MASK;
    GREEN_LED->PSOR |= GREEN_LED_MASK;
    BLUE_LED->PSOR |= BLUE_LED_MASK;
}

void set_rgb(int r, int g, int b)
{
    if (!r)
        RED_LED->PSOR |= RED_LED_MASK;
    else
        RED_LED->PCOR |= RED_LED_MASK;

    if (!g)
        GREEN_LED->PSOR |= GREEN_LED_MASK;
    else
        GREEN_LED->PCOR |= GREEN_LED_MASK;

    if (!b)
        BLUE_LED->PSOR |= BLUE_LED_MASK;
    else
        BLUE_LED->PCOR |= BLUE_LED_MASK;
}
