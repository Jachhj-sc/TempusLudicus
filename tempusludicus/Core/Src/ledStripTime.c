/**
    @file ledStripTime.c
    @author William Hak (william.hak57@gmail.com)
    @brief place for the led strip time draw functions
    @version 0.1
    @date 2023-12-14

    @copyright Copyright (c) 2023

 */
#include "ledStripTime.h"
#include "ledcontrol.h"
#include "timer_dma_ws28xx.h"
#include "unixFunction.h"
#include <math.h>

static datetime_t dateTime;

// color definitions
uint32_t seconds_color = COLOR_BLUE;
uint32_t minutes_color = COLOR_CHARTREUSE;
uint32_t hours_color = COLOR_CYAN;
uint32_t monthday_color = COLOR_AQUAMARINE;
uint32_t weekday_color = COLOR_ORANGE;

uint32_t return_bigger(uint32_t a, uint32_t b)
{
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

uint32_t return_smaller(uint32_t a, uint32_t b)
{
    if (a < b) {
        return a;
    } else {
        return b;
    }
}

void strip_drawTimeMood(uint64_t unix_timestamp, enum e_mood mood)
{
    switch (mood) {

    case RAINBOW:
        strip_drawTime_HUE(unix_timestamp);
        break;

    case DEFAULT_MOOD:
    default:
        strip_drawTimeSimple(unix_timestamp);

        break;
    }
}

void setStrip_TimeDrawMood_color(enum e_mood mood)
{
    switch (mood) {
    case COOL:
        seconds_color = COLOR_BLUE;
        minutes_color = COLOR_BLUE;
        hours_color = COLOR_BLUE;
        monthday_color = COLOR_BLUE;
        weekday_color = COLOR_BLUE;
        break;

    case WARM:
        seconds_color = COLOR_MAROON;
        minutes_color = COLOR_MAROON;
        hours_color = COLOR_MAROON;
        monthday_color = COLOR_MAROON;
        weekday_color = COLOR_MAROON;
        break;

    case EXCITED:
        seconds_color = COLOR_CRIMSON;
        minutes_color = COLOR_CRIMSON;
        hours_color = COLOR_CRIMSON;
        monthday_color = COLOR_CRIMSON;
        weekday_color = COLOR_CRIMSON;
        break;

    case MELLOW:
        seconds_color = COLOR_BLUE;
        minutes_color = COLOR_CHARTREUSE;
        hours_color = COLOR_CYAN;
        monthday_color = COLOR_AQUAMARINE;
        weekday_color = COLOR_ORANGE;
        break;

    case CHILL:
        seconds_color = COLOR_CYAN;
        minutes_color = COLOR_CYAN;
        hours_color = COLOR_CYAN;
        monthday_color = COLOR_CYAN;
        weekday_color = COLOR_CYAN;
        break;

    case DEFAULT_MOOD:
        seconds_color = COLOR_VIOLET;
        minutes_color = COLOR_CHARTREUSE;
        hours_color = COLOR_CYAN;
        monthday_color = COLOR_AQUAMARINE;
        weekday_color = COLOR_ORANGE;
        break;

    default:

        break;
    }
}

void strip_drawTimeSimple(uint64_t unix_timestamp)
{
    uint16_t startPos = 0;
    uint16_t endPos = 0;

    RTC_HAL_convert_unix_to_datetime(unix_timestamp, &dateTime);
    setStrip_clear();

    // draw seconds
    startPos = (STRIP_SECONDS_START > STRIP_SECONDS_END) ? STRIP_SECONDS_START + 1 : STRIP_SECONDS_START;
    endPos = (STRIP_SECONDS_START > STRIP_SECONDS_END) ? (startPos - dateTime.second) : (startPos + dateTime.second);
    setStrip_part(return_smaller(startPos, endPos), return_bigger(startPos, endPos), seconds_color);

    // draw minutes
    startPos = (STRIP_MINUTES_START > STRIP_MINUTES_END) ? STRIP_MINUTES_START + 1 : STRIP_MINUTES_START;
    endPos = (STRIP_MINUTES_START > STRIP_MINUTES_END) ? (startPos - dateTime.minute) : (startPos + dateTime.minute);
    setStrip_part(return_smaller(startPos, endPos), return_bigger(startPos, endPos), minutes_color);

    // draw hours
    startPos = (STRIP_HOURS_START > STRIP_HOURS_END) ? STRIP_HOURS_START + 1 : STRIP_HOURS_START;
    endPos = (STRIP_HOURS_START > STRIP_HOURS_END) ? (startPos - dateTime.hour) : (startPos + dateTime.hour);
    setStrip_part(return_smaller(startPos, endPos), return_bigger(startPos, endPos), hours_color);

    // draw monthday
    startPos = (STRIP_MONTHDAY_START > STRIP_MONTHDAY_END) ? STRIP_MONTHDAY_START + 1 : STRIP_MONTHDAY_START;
    endPos = (STRIP_MONTHDAY_START > STRIP_MONTHDAY_END) ? (startPos - dateTime.day) : (startPos + dateTime.day);
    setStrip_part(return_smaller(startPos, endPos), return_bigger(startPos, endPos), monthday_color);

    // draw weekday MONTHDAY TODO: fix this
    startPos = (STRIP_WEEKDAY_START > STRIP_WEEKDAY_END) ? STRIP_WEEKDAY_START + 1 : STRIP_WEEKDAY_START;
    endPos = (STRIP_WEEKDAY_START > STRIP_WEEKDAY_END) ? (startPos - ((((dateTime.day - 1) % 7) + 1) * 3))
                                                       : (startPos + ((((dateTime.day - 1) % 7) + 1) * 3));
    setStrip_part(return_smaller(startPos, endPos), return_bigger(startPos, endPos), weekday_color);

    Strip_send();
}

void strip_drawUltrasoneDistance(uint16_t distance_cm)
{
    if (distance_cm > LEDPIXELCOUNT) {
        distance_cm = LEDPIXELCOUNT;
    }

    setStrip_clear();

    for (uint16_t i = 0; i < distance_cm; i++) {
        setStrip_part(0, i, seconds_color);
    }

    Strip_send();
}

void strip_drawPensions(enum e_developer person, uint16_t distance_cm) {}

void strip_drawTime_HUE(uint64_t unix_timestamp)
{
    uint16_t startPos = 0;
    uint16_t endPos = 0;

    RTC_HAL_convert_unix_to_datetime(unix_timestamp, &dateTime);
    setStrip_clear();

    // draw seconds
    startPos = STRIP_SECONDS_START;
    endPos = (STRIP_SECONDS_START > STRIP_SECONDS_END) ? (startPos - dateTime.second) : (startPos + dateTime.second);
    setStrip_part(return_smaller(startPos, endPos), return_bigger(startPos, endPos), seconds_color);

    // draw minutes
    startPos = STRIP_MINUTES_START;
    endPos = (STRIP_MINUTES_START > STRIP_SECONDS_END) ? (startPos - dateTime.minute) : (startPos + dateTime.minute);
    setStrip_part(return_smaller(startPos, endPos), return_bigger(startPos, endPos), minutes_color);

    // draw hours
    startPos = STRIP_HOURS_START;
    endPos = (STRIP_HOURS_START > STRIP_HOURS_END) ? (startPos - dateTime.hour) : (startPos + dateTime.hour);
    setStrip_part(return_smaller(startPos, endPos), return_bigger(startPos, endPos), hours_color);

    // draw monthday
    startPos = STRIP_MONTHDAY_START;
    endPos = (STRIP_MONTHDAY_START > STRIP_MONTHDAY_END) ? (startPos - dateTime.day) : (startPos + dateTime.day);
    setStrip_part(return_smaller(startPos, endPos), return_bigger(startPos, endPos), monthday_color);

    // draw weekday MONTHDAY TODO: fix this
    startPos = STRIP_WEEKDAY_START;
    endPos = (STRIP_WEEKDAY_START > STRIP_WEEKDAY_END) ? (startPos - ((dateTime.day % 7) * 3))
                                                       : (startPos + ((dateTime.day % 7) * 3));
    setStrip_part(return_smaller(startPos, endPos), return_bigger(startPos, endPos), weekday_color);

    Strip_send();
}
