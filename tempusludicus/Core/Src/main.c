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
#include "uart0.h"

int main()
{
		uart0_init();
	
    init_rgb();

    init_sysTick();

    set_rgb(1, 0, 0);
    delay_us(1000000);
    set_rgb(0, 1, 0);
    delay_us(1000000);
    set_rgb(0, 0, 0);

    __enable_irq();

    while (1) {
    }
}
