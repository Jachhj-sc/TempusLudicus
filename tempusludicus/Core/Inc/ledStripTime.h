#ifndef LED_STRIP_TIME_H
#define LED_STRIP_TIME_H

#include "main.h"
#include "timer_dma_ws28xx.h"

/* 	Boundary configuration for the clock strip.
    The strip is one full snake of all the leds, of which every part has boundaries and fullfill different functions,
    there are 5 parts:
    1. the seconds part, first 60 leds of the strip, and is used to display the seconds (res 144
   Leds/M)
    2. the minutes part, next 60 leds of the strip, and is used to display the minutes (res 144 Leds/M)
    3. the hours part, next 24 leds of the strip, and is used to display the hours (res 60 Leds/M)
    4. the month-day part, next 31 leds of the strip, and is used to display the day (res 60 Leds/M)
    5. the week-day part, next 7 leds of the strip, and is used to display the week day (res 14 Leds/M)
*/

#define STRIP_SECONDS_START 0
#define STRIP_SECONDS_END   58

#define STRIP_MINUTES_START 118
#define STRIP_MINUTES_END   59

#define STRIP_HOURS_START 118
#define STRIP_HOURS_END   140

#define STRIP_MONTHDAY_START 171
#define STRIP_MONTHDAY_END   141

#define STRIP_WEEKDAY_START 172
#define STRIP_WEEKDAY_END   192

void setStrip_TimeDrawMood_color(enum e_mood mood);
void strip_drawTimeMood(uint64_t unix_timestamp, enum e_mood mood);

void strip_drawTimeSimple(uint64_t unix_timestamp);

void strip_drawUltrasoneDistance(uint16_t distance_cm);
void strip_drawPensions(enum e_developer person, uint16_t distance_cm);
void strip_drawTime_HUE(uint64_t unix_timestamp);

#endif // LED_STRIP_TIME_H
