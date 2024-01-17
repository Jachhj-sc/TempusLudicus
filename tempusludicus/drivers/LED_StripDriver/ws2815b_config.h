/**
    @file sk6812_config.h
    @author William Hak (william.hak57@gmail.com)
    @brief
    @version 0.1
    @date 2023-11-16

    @copyright Copyright (c) 2023

 */
#ifndef WS2815B_CONFIG_H
#define WS2815B_CONFIG_H

#include "main.h"

// #define RGBW
#define RGB

#define LEDPIXELCOUNT (194u)

#define STRIP_COUNT (1)

/*
    Timing specifications
    T0H 0 code ,high voltage time 	0.3us
    T0L 0 code, low voltage time 	0.9us
    T1H 1 code, high voltage time 	0.6us
    T1L 1 code, low voltage time 	0.6us

    RES  low voltage time 			>50µs(80µs)
*/
// in ns
#define STRIP_T0H  (400u)
#define STRIP_T0L  (850u)
#define STRIP_T1H  (650u)
#define STRIP_T1L  (600u)
#define STRIP_TTOT (1250u)

#define STRIP_RES (80000u)

#define STRIP_T_IS_SYMETRICAL (1)

#if ((STRIP_T0H + STRIP_T0L) != STRIP_TTOT) || ((STRIP_T1H + STRIP_T1L) != STRIP_TTOT)
#warning "timing is not symetrical, please check timing values (sk6812_config.h)"
#endif

// data structure
// 0x00GGRRBB

#endif /* WS2815B_CONFIG_H */
