/**
    @file main.c
    @author
    @brief

    @version 0.2
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
#include "uart0.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <MKL25Z4.h>


static uint32_t distance = 0;
static uint8_t switchstate = 1;


int main()
{
		
		init_rgb();
		tpm1_init();
		lcd_init();
		sw_init();
    init_sysTick();
  	uart0_init();
    __enable_irq();

		uint32_t unixtest = 1701608547;
		datetime_t unixTime;

		// systick init, for working with the ultrasoonsensor
		SysTick->LOAD = 0x00FFFFFF;
    SysTick->VAL = 0;
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
    SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;		
	

		// startup led 
    set_rgb(1, 0, 0);
    //delay_us(1000000);
    set_rgb(0, 1, 0);
    //delay_us(1000000);
    set_rgb(0, 0, 0);


		char text[80];

    while (1) 
		{
			
//			char c = uart0_get_char();
//			uart0_put_char(c);
			
			distance = read_distance(); // get the value of the ultrasoon sensor in cm 
			
			switchstate = get_switchState(); // get the value of wich button is pressed
			
			RTC_HAL_ConvertSecsToDatetime(&unixtest,&unixTime);

			switch(switchstate)
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
						sprintf(text, "distance cm = %d   ", distance);
						lcd_print(text);
						break;
						
				case 3:
						lcd_set_cursor(0,0);
						lcd_print("tijd <> pensioen");
						break;
			
				case 4:
						lcd_set_cursor(0,0);
						lcd_print("***debug***     ");
						break;
			
			
			}	
    }
}
