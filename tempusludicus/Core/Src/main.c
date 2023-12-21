/**
    @file main.c
    @author
    @brief

    @version 0.2
    @date	11/11/23

    @todo main logic bijschaven.
    @todo ledDraw functies maken. william
    @todo temp sensor implementeren. Maarten
    @todo

    @copyright Copyright (c) 2023
 */
#include "main.h"
#include "lcd_4bit.h"
#include "ledStripTime.h"
#include "pit.h"
#include "rgb.h"
#include "switches.h"
#include "tempSensor.h"
#include "uart0.h"
#include "ultrasonic_sensor.h"
#include "unixFunction.h"
#include "queue.h"
#include <ctype.h>
#include "updateValues.h"

static uint16_t distance_cm = 0;
static uint8_t switchstate = 1;

// variables for keeping track of time of ACTION
static uint32_t prevLCDUpdate = 0;
static uint32_t prevStripUpdate = 0;
static uint32_t prevPensionUpdate = 0;
static enum e_mood mood = NORMAL;
static enum e_developer person = 0;
static char sentDebuginfo = 0;

// Variable for mood setting
char moodSetting = 0;

int main(void)
{
    init_rgb();
    pit_init(); // Initialization of Periodic Interrupt Timer
    ultraS_sensor_init();
    lcd_init();
    sw_init();
    init_sysTick();
    uart0_init();
    init_adc_lm35();

    init_strip();
    __enable_irq();

    datetime_t DateTime;

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

        if (!q_empty(&RxQ)) {
					updateValue ();
				}
				else 
				{
				
				}
					
        ultraS_sensor_process();
        // uart_process();

        distance_cm = ultraS_get_distance_cm(); // get the value of the ultrasoon sensor in cm
        switchstate = get_switchState();        // get the value of wich button is pressed
        // uncomment the following code to set a fixed state for debugging purposes
        // switchstate = DRAWSTRIP; // or any other state

        RTC_HAL_ConvertSecsToDatetime(&unix_timestamp, &DateTime);

        switch (switchstate) {

        case DRAWSTRIP:
				{
					sentDebuginfo = 0;
				}
        case TIMELCD:
						sentDebuginfo = 0;
            if (get_millis() > prevStripUpdate + 100) {
                strip_drawTimeMood(unix_timestamp, mood);
                prevStripUpdate = get_millis();
            }

            // update the lcd every second
            if (get_millis() > prevLCDUpdate + 1000) {
                LCD_putDateTime(DateTime);
                prevLCDUpdate = get_millis();
            }
            break;

        case ULTRASOON:
						sentDebuginfo = 0;
            lcd_set_cursor(0, 0);
            lcd_print("ultrasoon sensor");
            lcd_set_cursor(0, 1);
            sprintf(text, "distance cm = %d   ", distance_cm);
            strip_drawUltrasoneDistance(distance_cm);
            lcd_print(text);
            break;

        case PENSIOEN:
						sentDebuginfo = 0;
            lcd_set_cursor(0, 0);
            lcd_print("tijd <> pensioen");
            strip_drawPensions(person, distance_cm);

            if (get_millis() > prevPensionUpdate + 2000) {
                person;
                if (person >= developer_amount) {
                    person = 0;
                }
                prevPensionUpdate = get_millis();
            }
            break;

        case DEBUG:

						
            lcd_set_cursor(0, 0);
            lcd_print("***debug***     ");
						if (sentDebuginfo == 0){
							
						// Send unix timestamp
							uart0_put_char('U');
						uart0_send_uint32(unix_timestamp);
							uart0_put_char('S');
							
							//Send distance
							uart0_put_char('D');
						uart0_send_uint32(distance_cm);
							uart0_put_char('S');
							
							// Send mood 
							uint32_t debugMood = mood;
							
							uart0_put_char('M');
						uart0_send_uint32(debugMood);
							uart0_put_char('S');
							
							// Send temperature 
            uint16_t adc_result = read_adc_lm35();
            float temperature = calculate_temperature_from_lm35(adc_result);
            addTemperatureToBuffer(temperature);
            float averageTemperature = calculateAverageTemperature();							
							

							uart0_put_char('T');
						uart0_send_float(averageTemperature);
							uart0_put_char('S');
							
							// Block 
						sentDebuginfo = 1;
						}

            // if object detected turn on strip
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
            break;

        case TEMPSENSOR: {
            uint16_t adc_result = read_adc_lm35();
            float temperature = calculate_temperature_from_lm35(adc_result);
            addTemperatureToBuffer(temperature);
            float averageTemperature = calculateAverageTemperature();

            lcd_set_cursor(0, 0);
            sprintf(text, "Temp: %.2f C   ", averageTemperature);
            lcd_print(text);
        } break;
	

        }
    }
}
