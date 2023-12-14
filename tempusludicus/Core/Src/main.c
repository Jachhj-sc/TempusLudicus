/**
    @file main.c
    @author
    @brief

    @version 0.2
    @date	11/11/23

    @copyright Copyright (c) 2023
 */
#include "main.h"

#include "lcd_4bit.h"
#include "rgb.h"
#include "switches.h"
#include "timer_dma_ws2812.h"
#include "uart0.h"
#include "ultrasonic_sensor.h"
#include "unixFunction.h"

static uint32_t distance_cm = 0;
static uint8_t switchstate = 1;

int main()
{
    init_rgb();
    ultraS_sensor_init();
    // lcd_init();
    sw_init();
    init_sysTick();
    uart0_init();

    init_strip();
    __enable_irq();

    uint32_t unixtest = 1701608547;
    datetime_t unixTime;

    char text[80];

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

    while (1) {

        //			char c = uart0_get_char();
        //			uart0_put_char(c);

        ultraS_sensor_process();
        distance_cm = ultraS_get_distance_cm(); // get the value of the ultrasoon sensor in cm

        switchstate = get_switchState(); // get the value of wich button is pressed

        RTC_HAL_ConvertSecsToDatetime(&unixtest, &unixTime);

        static uint32_t prevStripUpdate = 0;
        if (get_millis() > prevStripUpdate + 100) {
            if (distance_cm < 10) {
                setStrip_all(color32(0, 3, 0, 2));
                Strip_send();
                set_rgb(0, 1, 1);
            } else {
                setStrip_all(0);
                Strip_send();
                set_rgb(0, 0, 0);
            }
            prevStripUpdate = get_millis();
        }

        switch (switchstate) {
        case 1:
            // LCD_putDateTime(unixTime);
            // unixtest++;
            // delay_us(1000000);
            break;

        case 2:
            lcd_set_cursor(0, 0);
            lcd_print("ultrasoon sensor");
            lcd_set_cursor(0, 1);
            sprintf(text, "distance cm = %d   ", distance_cm);
            lcd_print(text);
            break;

        case 3:
            lcd_set_cursor(0, 0);
            lcd_print("tijd <> pensioen");
            break;

        case 4:
            lcd_set_cursor(0, 0);
            lcd_print("***debug***     ");
            break;
        }
    }
}
