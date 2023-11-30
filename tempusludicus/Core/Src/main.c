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

int main()
{
    init_rgb();
    init_sysTick();
	init_strip();

    set_rgb(1, 0, 0);
    delay_us(1000000);
    set_rgb(0, 1, 0);
    delay_us(1000000);
    set_rgb(0, 0, 0);

    __enable_irq();

    setStrip_clear();
    Strip_send();

    while (1) {
        setStrip_all(color32(255, 0, 0, 0));
        Strip_send();
        _delay_ms(1000);
        setStrip_all(color32(0, 0, 0, 0));
        _delay_ms(1000);
        Strip_send();
    }
}
