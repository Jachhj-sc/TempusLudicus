/*
 *	WS2812_CONFIG.h
 *
 * Created: 13/07/2021 16:20:40
 *  Author: william
 */

#ifndef WS2812_CONFIG_H
#define WS2812_CONFIG_H

#include "main.h"

///////////////////////////////////////////////////////////////////////
// Define I/O pin
///////////////////////////////////////////////////////////////////////

//#define ws2812_port B // Data port
//#define ws2812_pin  2 // Data out pin

#define LEDPIXELCOUNT (179) /*182*/

/*
    Timing specifications
    T0H 0 code ,high voltage time 	0.35us 	±150ns
    T0L 0 code, low voltage time 	0.8us 	±150ns

    T1H 1 code, high voltage time 	0.7us 	±150ns
    T1L 1 code, low voltage time 	0.6us 	±150ns

    RES  low voltage time 	 		>50µs
*/
#define ws2812_T0H (350)
#define ws2812_T0L (800)
#define ws2812_T1H (700)
#define ws2812_T1L (600)
#define ws2812_RES (80000)

// data structure
//  0x00GGRRBB

#endif /* WS2812_CONFIG_H_ */
