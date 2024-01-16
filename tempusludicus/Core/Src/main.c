/**
    @file main.c
    @author
    @brief

    @version 0.2
    @date	11/11/23

    @copyright Copyright (c) 2023
 */
#include "main.h"

#include "brightness_pot.h"
#include "lcd_4bit.h"
#include "ledStripTime.h"
#include "pit.h"
#include "queue.h"
#include "rgb.h"
#include "switches.h"
#include "tempSensor.h"
#include "uart0.h"
#include "ultrasonic_sensor.h"
#include "unixFunction.h"
#include "updateValues.h"

#define MIN_TEMPERATURE 2.0f
#define MAX_TEMPERATURE 120.0f

void mainProcess();
void device_test_sequence(void);
void process_button_state(enum e_switchState switchstate);

volatile system_state_t system_state;
static uint32_t display_update_pause = 0;

char text[80];

int main(void)
{
    init_rgb();
    pit_init();
    ultraS_sensor_init();
    sw_init();
    uart0_init();
    init_adc_lm35();
    init_brightness_pot();
    init_strip();

    init_sysTick();
    __enable_irq();

    lcd_init();

    setStrip_Brightness(3);
    setStrip_clear();
    Strip_send();
    set_rgb(0, 0, 0);
    _delay_ms(10);

    device_test_sequence();

    system_state.mood = DEFAULT_MOOD;
    system_state.person = 0;
    system_state.switchState = NO_SWITCH_PRESSED;
    system_state.programState = DRAWSTRIP;
    setStrip_TimeDrawMood_color(system_state.mood);

    while (1) {
        process_uart();
        process_ultrasonic_sensor();
        process_button_state(get_switch_state());

        mainProcess();
    }
}

void mainProcess()
{
    datetime_t DateTime;

    static uint32_t prev_strip_update = 0;
    static uint32_t prev_ultrasonic_update = 0;
    static uint32_t prev_pension_update = 0;
    static uint32_t prev_temp_update = 0;

    uint16_t distance_cm = get_ultrasonic_distance_cm();
    RTC_HAL_convert_unix_to_datetime(get_unix_timestamp(), &DateTime);

    switch (system_state.programState) {
    case DRAWSTRIP: {
        if (get_millis() > prev_strip_update + 100) {

            if (get_millis() > display_update_pause) {
                LCD_putDateTime(DateTime);
            }

            // update strip
            uint8_t brightness = get_brightness_pot_value();
            setStrip_Brightness(brightness);
            strip_drawTimeMood(get_unix_timestamp(), system_state.mood);

            prev_strip_update = get_millis();
        }
        break;
    }

    case ULTRASOON: {
        if (get_millis() > prev_ultrasonic_update + 80) {
            if (get_millis() > display_update_pause) {
                lcd_set_cursor(0, 0);
                lcd_print("ultrasoon sensor");

                lcd_set_cursor(0, 1);
                sprintf(text, "cm = %d         ", distance_cm);
                lcd_print(text);
            }

            strip_drawUltrasoneDistance(distance_cm);

            prev_ultrasonic_update = get_millis();
        }
        break;
    }

    case PENSIOEN: {
        if (get_millis() > prev_pension_update + 2000) {
            system_state.person++;
            if (system_state.person >= DEVELOPER_AMOUNT) {
                system_state.person = 0;
            }
            prev_pension_update = get_millis();
        }

        if (get_millis() > prev_strip_update + 100) {
            if (get_millis() > display_update_pause) {
                lcd_set_cursor(0, 0);
                lcd_print("tijd <> pensioen");

                lcd_set_cursor(0, 1);
                lcd_print("                ");
            }

            strip_drawPensions(system_state.person, distance_cm);
            prev_strip_update = get_millis();
        }
        break;
    }

    case DEBUG: {
        // if object detected turn on strip
        if (get_millis() > prev_strip_update + 100) {
            if (get_millis() > display_update_pause) {
                lcd_set_cursor(0, 0);
                lcd_print("***debug***     ");

                lcd_set_cursor(0, 1);
                lcd_print("                ");
            }

            // Send unix timestamp
            uart0_put_char('U');
#warning "this typecast wont work in 30 years! for the future make the function 64-bit capable"
            uart0_send_uint32((uint32_t)get_unix_timestamp());
            uart0_put_char('S');

            // Send distance
            uart0_put_char('D');
            uart0_send_uint32(distance_cm);
            uart0_put_char('S');

            // Send mood
            uart0_put_char('M');
            uart0_send_uint32(system_state.mood);
            uart0_put_char('S');

            // Send temperature
            float averageTemperature = get_average_temperature();

            uart0_put_char('T');
            uart0_send_float(averageTemperature);
            uart0_put_char('S');

            if (distance_cm < 10 && distance_cm > 0) {
                setStrip_all(color32(255, 255, 0, 0));
                Strip_send();
                set_rgb(0, 1, 1);
            } else {
                setStrip_all(0);
                Strip_send();
                set_rgb(0, 0, 0);
            }
            prev_strip_update = get_millis();
        }
        break;
    }

    case TEMPSENSOR: {
        if (get_millis() > prev_temp_update + 100) {
            if (get_millis() > display_update_pause) {
                lcd_set_cursor(0, 0);
                sprintf(text, "Temp: %.1f C    ", get_average_temperature());
                lcd_print(text);

                lcd_set_cursor(0, 1);
                lcd_print("                ");
            }
            prev_temp_update = get_millis();
        }
        break;
    }

    case PROGRAMSTATE_AMOUNT:
    default:
        break;
    }
}

void device_test_sequence(void)
{
    // startup blink
    setStrip_all(color32(255, 0, 0, 0));
    Strip_send();
    set_rgb(1, 0, 0);
    _delay_ms(500);

    setStrip_all(color32(0, 255, 0, 0));
    Strip_send();
    set_rgb(0, 1, 0);
    _delay_ms(500);

    setStrip_all(color32(0, 0, 255, 0));
    Strip_send();
    set_rgb(0, 0, 1);
    _delay_ms(500);

    setStrip_all(color32(255, 255, 255, 0));
    Strip_send();
    set_rgb(1, 1, 1);

    _delay_ms(1000);

    setStrip_clear();
    Strip_send();
    set_rgb(0, 0, 0);
}

void process_button_state(enum e_switchState switchstate)
{
    switch (switchstate) {
    case SWITCH_1_PRESSED:
        system_state.programState++;

        if (system_state.programState >= PROGRAMSTATE_AMOUNT) {
            system_state.programState = 0;
        }

        break;

    case SWITCH_2_PRESSED:
        system_state.mood++;
        if (system_state.mood >= MOOD_AMOUNT) {
            system_state.mood = 0;
        }

        setStrip_TimeDrawMood_color(system_state.mood);

        lcd_set_cursor(0, 0);
        sprintf(text, "MOOD = %d        ", system_state.mood);
        lcd_print(text);

        lcd_set_cursor(0, 1);
        lcd_print(e_mood_name[system_state.mood]);

        display_update_pause = get_millis() + 2000;
        break;

    case SWITCH_1_2_PRESSED:
        // run test sequence
        device_test_sequence();
        break;

    default:
        break;
    }
}

void HardFault_Handler(void)
{
    while (1) {
        set_rgb(1, 0, 0);
        delay_us(500000);

        set_rgb(0, 0, 0);
        delay_us(500000);
    }
}
