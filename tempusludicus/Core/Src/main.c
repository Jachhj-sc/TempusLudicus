/**
    @file main.c
    @author
    @brief

    @version 0.2
    @date	11/11/23

    @copyright Copyright (c) 2023
 */
#include "main.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <MKL25Z4.h>

#include "delay.h"
#include "rgb.h"
#include "sysTick.h"
#include "lcd_4bit.h"
#include "unixFunction.h"
#include "tpm1.h"
#include "switches.h"
#include "uart0.h"
#include "timer_dma_ws2812.h"

volatile uint32_t teller = 0;
volatile uint8_t choice = 1;

int main()
{
		init_rgb();
		tpm1_init();
		lcd_init();
		sw_init();
    init_sysTick();
  	uart0_init();

    init_strip();
    __enable_irq();

  	uint32_t unixtest = 1701608547;
		datetime_t unixTime;

		// systick init, for working with the ultrasoonsensor
		SysTick->LOAD = 0x00FFFFFF;
    SysTick->VAL = 0;
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
    SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;		
	

      // startup blink
    setStrip_all(color32(2, 3, 0, 0));
    Strip_send();
    set_rgb(1, 0, 0);
    _delay_ms(200);

    setStrip_all(color32(0, 3, 2, 0));
    Strip_send();
    set_rgb(0, 1, 0);
    _delay_ms(200);

    setStrip_all(color32(0, 3, 0, 2));
    Strip_send();
    set_rgb(0, 1, 1);
    _delay_ms(200);

    setStrip_clear();
    Strip_send();
    set_rgb(0, 0, 0);

		char text[80];

    while (1) 
		{
			
			char c = uart0_get_char();
			uart0_put_char(c);
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
