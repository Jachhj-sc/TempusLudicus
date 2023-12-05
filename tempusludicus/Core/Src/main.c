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
#include "tpm1.h"
#include "switches.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <MKL25Z4.h>


volatile uint32_t teller = 0;
volatile uint8_t choice = 1;

int main()
{
		
		init_rgb();
		tpm1_init();
		lcd_init();
		sw_init();
 //   init_sysTick();
		
		uint32_t unixtest = 1701608547;
		datetime_t unixTime;


		SysTick->LOAD = 0x00FFFFFF;
    SysTick->VAL = 0;
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
    SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;		

		
		lcd_clear();
    lcd_bl_pwmcontrol(0x2000);

    set_rgb(1, 0, 0);
    delay_us(1000000);
    set_rgb(0, 1, 0);
    delay_us(1000000);
    set_rgb(0, 0, 0);

    __enable_irq();
		

		
		char text[80];

    while (1) {
	
		
		RTC_HAL_ConvertSecsToDatetime(&unixtest,&unixTime);

			switch(choice)
			{
			case 1:
			LCD_putDateTime(unixTime);
			unixtest++;
			delay_us(1000000);
			break;
			
			case 2:
			lcd_set_cursor(0,0);
			lcd_print("ultrasoon sensor");
			lcd_set_cursor(0,1);
			sprintf(text, "distance cm = %d   ", teller);
			lcd_print(text);
			break;
			
			case 5:
			lcd_clear();
			lcd_set_cursor(0,0);
			lcd_print("***debug***");
			break;
			case 6:
			choice = 1;
			break;
			
			}	
		
    }
}
