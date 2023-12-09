/**
    @file main.c
    @author
    @brief

    @version 0.1
    @date	11/11/23

    @copyright Copyright (c) 2023
 */
#include "main.h"

#include "delay.h"
#include "rgb.h"
#include "sysTick.h"

#include "timer_dma_ws2812.h"

int main()
{
    init_rgb();
    init_sysTick();

    init_strip();
    __enable_irq();




    // startup blink
    setStrip_all(color32(2, 3, 0, 0));
    Strip_send();
    _delay_ms(200);

    setStrip_all(color32(0, 3, 2, 0));
    Strip_send();
    _delay_ms(200);

    setStrip_all(color32(0, 3, 0, 2));
    Strip_send();
    _delay_ms(200);

    setStrip_clear();
    Strip_send();

    while (1) {
    }
}
