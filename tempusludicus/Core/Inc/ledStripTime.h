#ifndef LED_STRIP_TIME_H
#define LED_STRIP_TIME_H

#include "common.h"
#include "timer_dma_ws2812.h"

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

#define STRIP_SECONDS_START  0
#define STRIP_SECONDS_END    59
#define STRIP_MINUTES_START  60
#define STRIP_MINUTES_END    119
#define STRIP_HOURS_START    120
#define STRIP_HOURS_END      143
#define STRIP_MONTHDAY_START 144
#define STRIP_MONTHDAY_END   174
#define STRIP_WEEKDAY_START  175
#define STRIP_WEEKDAY_END    181

// color definitions
#define SECONDS_COLOR  COLOR_BLUE
#define MINUTES_COLOR  COLOR_CHARTREUSE
#define HOURS_COLOR    COLOR_CYAN
#define MONTHDAY_COLOR COLOR_AQUAMARINE
#define WEEKDAY_COLOR  COLOR_ORANGE

enum e_mood {
    NORMAL,
    COOL,
    WARM,
    EXCITED,
    MELLOW,
    CHILL,
	MoodAmount
};

enum e_developer {
    Kevin,
    Roel,
    Maarten,
    William,
    developer_amount
};

void strip_drawTimeMood(uint32_t unix_timestamp, enum e_mood mood);
void strip_drawTimeSimple(uint32_t unix_timestamp);
void strip_drawTimeCool(uint32_t unix_timestamp);
void strip_drawTimeWarm(uint32_t unix_timestamp);
void strip_drawTimeExcited(uint32_t unix_timestamp);
void strip_drawTimeMellow(uint32_t unix_timestamp);
void strip_drawTimeChill(uint32_t unix_timestamp);

void strip_drawUltrasoneDistance(uint16_t distance_cm);
void strip_drawPensions(enum e_developer person, uint16_t distance_cm);

#endif // LED_STRIP_TIME_H
