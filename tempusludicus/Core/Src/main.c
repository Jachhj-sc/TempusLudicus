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

#include "rgbStrip.h"
#include "sysTick.h"
#include "timer_dma_ws2812.h"

int main()
{
    init_rgb();
    init_sysTick();

    init_strip();
    __enable_irq();

    setStrip_clear();
    Strip_send();
    strip_sendContinuous();

    setStrip_all(color32(10, 10, 10, 10));

    while (1) {
        setStrip_all(color32(2, 0, 0, 0));
        Strip_send();
        set_rgb(1, 0, 0);
        //_delay_ms(2000);
        delay_us(500000);

        setStrip_all(color32(0, 2, 0, 0));
        Strip_send();
        set_rgb(1, 0, 0);
        //_delay_ms(2000);
        delay_us(500000);

        setStrip_all(color32(0, 0, 2, 0));
        Strip_send();
        set_rgb(1, 0, 0);
        //_delay_ms(2000);
        delay_us(500000);

        setStrip_all(color32(0, 0, 0, 2));
        Strip_send();
        set_rgb(1, 0, 0);
        //_delay_ms(2000);
        delay_us(500000);

        setStrip_all(color32(0, 0, 0, 0));
        Strip_send();
        set_rgb(0, 1, 0);
        //_delay_ms(2000);
        delay_us(500000);

        effect_snakeGrow_b(1, color32(2, 2, 0, 0));
    }
}
