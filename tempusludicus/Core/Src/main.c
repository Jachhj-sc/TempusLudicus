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

int main(void)
{
    init_sysTick();
    __enable_irq();

    init_rgb();
    pit_init(); // Initialization of Periodic Interrupt Timer
    ultraS_sensor_init();
    lcd_init();
    sw_init();
    uart0_init();
    init_adc_lm35();
    init_brightness_pot();
    init_strip();

    setStrip_Brightness(3);
    setStrip_clear();
    Strip_send();
    set_rgb(0, 0, 0);
    _delay_ms(10);

    device_test_sequence();

    system_state.effect = STATIC;
    system_state.mood = DEFAULT_MOOD;
    system_state.person = 0;
    system_state.switchState = NO_SWITCH_PRESSED;
    system_state.programState = DRAWSTRIP;
    setStrip_TimeDrawMood(system_state.mood);

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
    char text[80];

    static uint32_t prevStripUpdate = 0;
    static uint32_t prevPensionUpdate = 0;

    uint16_t distance_cm = get_ultrasonic_distance_cm();
    RTC_HAL_convert_unix_to_datetime(get_unix_timestamp(), &DateTime);

    switch (system_state.programState) {
    case DRAWSTRIP:
    case TIMELCD: {
        if (get_millis() > prevStripUpdate + 100) {
            // update lcd
            LCD_putDateTime(DateTime);

            // update strip
            uint8_t brightness = get_brightness_pot_value();
            setStrip_Brightness(brightness);
            strip_drawTimeEffect(get_unix_timestamp(), system_state.effect);
            prevStripUpdate = get_millis();
        }
        break;
    }

    case ULTRASOON: {
        lcd_set_cursor(0, 0);
        lcd_print("ultrasoon sensor");
        lcd_set_cursor(0, 1);

        sprintf(text, "distance cm = %d   ", distance_cm);
        lcd_print(text);

        strip_drawUltrasoneDistance(distance_cm);
        break;
    }

    case PENSIOEN: {
        lcd_set_cursor(0, 0);
        lcd_print("tijd <> pensioen");
        strip_drawPensions(system_state.person, distance_cm);

        if (get_millis() > prevPensionUpdate + 2000) {
            system_state.person++;
            if (system_state.person >= DEVELOPER_AMOUNT) {
                system_state.person = 0;
            }
            prevPensionUpdate = get_millis();
        }
        break;
    }

    case DEBUG: {
        // if object detected turn on strip
        if (get_millis() > prevStripUpdate + 100) {

            lcd_set_cursor(0, 0);
            lcd_print("***debug***     ");

            // Send unix timestamp
            uart0_put_char('U');
            uart0_send_uint32(get_unix_timestamp());
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
            uint16_t adc_result = (uint16_t)read_adc_lm35();
            float temperature = calculate_temperature_from_lm35(adc_result);
            addTemperatureToBuffer(temperature);
            float averageTemperature = calculateAverageTemperature();

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
            prevStripUpdate = get_millis();
        }
        break;
    }

    case TEMPSENSOR: {
        uint16_t adc_result = (uint16_t)read_adc_lm35();
        float temperature = calculate_temperature_from_lm35(adc_result);
        if (temperature < MIN_TEMPERATURE || temperature > MAX_TEMPERATURE) {
            lcd_set_cursor(0, 0);
            lcd_print("  Error!  ");
        } else {
            addTemperatureToBuffer(temperature);
            float averageTemperature = calculateAverageTemperature();
            lcd_set_cursor(0, 0);
            sprintf(text, "Temp: %.2f C   ", averageTemperature);
            lcd_print(text);
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
        if (system_state.programState < PROGRAMSTATE_AMOUNT) {
            system_state.programState++;
        } else {
            system_state.programState = 0;
        }
        break;

    case SWITCH_2_PRESSED:
        if (system_state.mood < MOOD_AMOUNT) {
            system_state.mood++;
        } else {
            system_state.mood = 0;
        }
        setStrip_TimeDrawMood(system_state.mood);
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
