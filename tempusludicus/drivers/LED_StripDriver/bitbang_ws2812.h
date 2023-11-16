/*
 * light weight WS2812 lib include
 *
 * Version 2.3  - Nev 29th 2015
 * Author: Tim (cpldcpu@gmail.com)
 *
 * Please do not change this file! All configuration is handled in "ws2812_config.h"
 *
 * License: GNU GPL v2+ (see License.txt)
 +
 */

#ifndef LIGHT_WS2812_H_
#define LIGHT_WS2812_H_

#include "main.h"
#include "ws2812_config.h"

///////////////////////////////////////////////////////////////////////
// Define Reset time in �s.
//
// This is the time the library spends waiting after writing the data.
//
// WS2813 needs 300 �s reset time
// WS2812 and clones only need 50 �s
//
///////////////////////////////////////////////////////////////////////
#ifndef ws2812_resettime
#define ws2812_resettime 300
#endif

///////////////////////////////////////////////////////////////////////
// Define I/O pin
///////////////////////////////////////////////////////////////////////
#ifndef ws2812_port
#define ws2812_port B // Data port
#endif

#ifndef ws2812_pin
#define ws2812_pin 2 // Data out pin
#endif

/*
 *  Structure of the LED array
 *
 * cRGB:     RGB  for WS2812S/B/C/D, SK6812, SK6812Mini, SK6812WWA, APA104, APA106
 * cRGBW:    RGBW for SK6812RGBW
 */

struct cRGB {
    uint8_t g;
    uint8_t r;
    uint8_t b;
};
struct cRGBW {
    uint8_t g;
    uint8_t r;
    uint8_t b;
    uint8_t w;
};

/* User Interface
 *
 * Input:
 *         ledarray:           An array of GRB data describing the LED colors
 *         number_of_leds:     The number of LEDs to write
 *         pinmask (optional): Bitmask describing the output bin. e.g. _BV(PB0)
 *
 * The functions will perform the following actions:
 *         - Set the data-out pin as output
 *         - Send out the LED data
 *         - Wait 50�s to reset the LEDs
 */

void ws2812_setleds(struct cRGB *ledarray, uint16_t number_of_leds);
void ws2812_setleds_pin(struct cRGB *ledarray, uint16_t number_of_leds, uint8_t pinmask);
void ws2812_setleds_rgbw(struct cRGBW *ledarray, uint16_t number_of_leds);

/*
 * Old interface / Internal functions
 *
 * The functions take a byte-array and send to the data output as WS2812 bitstream.
 * The length is the number of bytes to send - three per LED.
 */

void ws2812_sendarray(uint8_t *array, uint16_t length);
void ws2812_sendarray_mask(uint8_t *array, uint16_t length, uint32_t pinmask);

/*
 * Internal defines
 */
#if !defined(CONCAT)
#define CONCAT(a, b) a##b
#endif

#if !defined(CONCAT3)
#define CONCAT3(a, b, c) a##b##c
#endif

#if !defined(CONCAT_EXP)
#define CONCAT_EXP(a, b) CONCAT(a, b)
#endif

#if !defined(CONCAT_EXP3)
#define CONCAT_EXP3(a, b, c) CONCAT3(a, b, c)
#endif

#define ws2812_PORTREG CONCAT_EXP3(GPIO, ws2812_port, _PDOR)
#define ws2812_DDRREG  CONCAT_EXP3(GPIO, ws2812_port, _PDDR)

#define MASK(x) (1 << x)

#endif /* LIGHT_WS2812_H_ */
