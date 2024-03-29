/*! ***************************************************************************
 *
 * \brief     Low level driver for the switches
 * \file      switches.h
 * \author    Hugo Arends
 * \date      July 2021
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
#ifndef SWITCHES_H
#define SWITCHES_H

#include <MKL25Z4.h>
#include <stdbool.h>
#include "main.h"

/// \name SW1
/// \{
#define PIN_SW1_PORT  PORTD
#define PIN_SW1_PT    PTD
#define PIN_SW1_SHIFT (0)
#define PIN_SW1       (1 << PIN_SW1_SHIFT)
/// \}

/// \name SW2
/// \{
#define PIN_SW2_PORT  PORTD
#define PIN_SW2_PT    PTD
#define PIN_SW2_SHIFT (2)
#define PIN_SW2       (1 << PIN_SW2_SHIFT)
/// \}

//#define BUTTON_STATE_KEEP_HIGH_TIME 500ul
#define DEBOUNCE_TIME               50ul // ms
/// The number of keys available

/// Defines the type for the keys
extern volatile uint8_t buttonState_1;
extern volatile uint8_t buttonState_2;

// Function prototypes
void sw_init(void);
enum e_switchState get_switch_state(void);

#endif // SWITCHES_H
