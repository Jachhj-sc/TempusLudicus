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
#include "lcd_4bit.h"
#include "unixFunction.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <MKL25Z4.h>

int main()
{
    init_rgb();
		
		lcd_init();

    init_sysTick();
		
		uint32_t unixtest = 17001550850;
		datetime_t unixTime;
		
		lcd_clear();
    lcd_bl_pwmcontrol(0x2000);

    set_rgb(1, 0, 0);
    delay_us(1000000);
    set_rgb(0, 1, 0);
    delay_us(1000000);
    set_rgb(0, 0, 0);

    __enable_irq();

    while (1) {
		RTC_HAL_ConvertDatetimeToSecs(&unixTime,&unixtest);
		
		lcd_putdatetime(unixTime);
		

		
		
    }
}
