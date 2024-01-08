#ifndef BRIGHTNESS_POT_H
#define BRIGHTNESS_POT_H

#include "common.h"

// ----------------------------------------------------------------------------
// potentiometer hardware mappings
/// \name B_POT
/// \{
#define PIN_B_POT_PORT PORTD
#define PIN_B_POT_PT   PTD
#define PIN_B_POT_SHIFT (6)
#define PIN_B_POT       (1 << PIN_B_POT_SHIFT)
/// \}

#define MAX_BRIGHTNESS 218
void init_brightness_pot(void);
uint8_t get_brightness_pot_value(void);

#endif // BRIGHTNESS_POT_H