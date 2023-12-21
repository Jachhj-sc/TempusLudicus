/*! ***************************************************************************
 *
 * \brief     Low level driver for the switches
 * \file      switches.c
 * \author    Hugo Arends
 * \date      June 2021
 *
 * \copyright 2021 HAN University of Applied Sciences. All Rights Reserved.
 *            \n\n
 *            Permission is hereby granted, free of charge, to any person
 *            obtaining a copy of this software and associated documentation
 *            files (the "Software"), to deal in the Software without
 *            restriction, including without limitation the rights to use,
 *            copy, modify, merge, publish, distribute, sublicense, and/or sell
 *            copies of the Software, and to permit persons to whom the
 *            Software is furnished to do so, subject to the following
 *            conditions:
 *            \n\n
 *            The above copyright notice and this permission notice shall be
 *            included in all copies or substantial portions of the Software.
 *            \n\n
 *            THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *            EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *            OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *            NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *            HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *            WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *            FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *            OTHER DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/
#include "switches.h"
#include "common.h"
#include "lcd_4bit.h"
#include "main.h"
#include "ultrasonic_sensor.h"

#include <stdio.h>

volatile uint8_t buttonState_1 = 0;
volatile uint8_t buttonState_2 = 0;

/*!
 * \brief Initialises the switches on the shield
 *
 * This functions initializes the switches on the shield.
 */
void sw_init(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;

    // interrupt handlers config

    // switch 1
    PORTD->PCR[0] = PORT_PCR_IRQC(11) | PORT_PCR_MUX(1) | PORT_PCR_PS(1) | PORT_PCR_PE(1);

    // switch 2
    PORTD->PCR[3] = PORT_PCR_IRQC(11) | PORT_PCR_MUX(1) | PORT_PCR_PS(1) | PORT_PCR_PE(1);

    NVIC_SetPriority(PORTD_IRQn, 0);
    NVIC_ClearPendingIRQ(PORTD_IRQn);
    NVIC_EnableIRQ(PORTD_IRQn);
}

enum e_switchState get_switchState(void)
{
    enum e_switchState state = NO_SWITCH_PRESSED;
    static uint8_t timerStarted = 1;
    static uint32_t debounceStartTime = 0;

	// start the debounce timer
    if (((buttonState_1 == 1) || (buttonState_2 == 1)) && !timerStarted) {
        debounceStartTime = get_millis();
        timerStarted = 1;
    }

	// wait until a certain amount of time is passed since the last buttonspress
	// to be sure to catch both buttons to be pressed
    if (get_millis() > debounceStartTime + DEBOUNCE_TIME) {
        if ((buttonState_1 == 1) && (buttonState_2 == 1)) {
            state = SWITCH_1_2_PRESSED;
        } else if (buttonState_1 == 1) {
            state = SWITCH_1_PRESSED;
        } else if (buttonState_2 == 1) {
            state = SWITCH_2_PRESSED;
        }

        buttonState_1 = 0;
        buttonState_2 = 0;
        timerStarted = 0;
    }

    return state;
}

/// moved interrupt handlers to it_handlers.c
