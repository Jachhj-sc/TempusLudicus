/**
    @file sk6812_config.h
    @author William Hak (william.hak57@gmail.com)
    @brief 
    @version 0.1
    @date 2023-11-16
    
    @copyright Copyright (c) 2023
    
 */
#ifndef SK6812_CONFIG_H
#define SK6812_CONFIG_H

#include "main.h"

///////////////////////////////////////////////////////////////////////
// Define I/O pin
///////////////////////////////////////////////////////////////////////

#define sk6812_port C // Data port
#define sk6812_pin  5 // Data out pin

#define LEDPIXELCOUNT (30) /*46*/

/*
    Timing specifications
    T0H 0 code ,high voltage time 	0.3us 	±150ns
    T0L 0 code, low voltage time 	0.9us 	±150ns

    T1H 1 code, high voltage time 	0.6us 	±150ns
    T1L 1 code, low voltage time 	0.6us 	±150ns

    RES  low voltage time 			>50µs(80µs)
*/
//in ns
#define sk6812_T0H (300)
#define sk6812_T0L (900)
#define sk6812_T1H (600)
#define sk6812_T1L (600)
#define sk6812_RES (80000)

// data structure
// 0x00GGRRBB

#endif /* SK6812_CONFIG_H */
