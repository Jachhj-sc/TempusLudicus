/**
    @file ledStripTime.c
    @author William Hak (william.hak57@gmail.com)
    @brief place for the led strip time draw functions
    @version 0.1
    @date 2023-12-14

    @copyright Copyright (c) 2023

 */
#include "ledStripTime.h"
#include "lcd_4bit.h"
#include "ledcontrol.h"
#include "timer_dma_ws28xx.h"
#include "unixFunction.h"
#include <math.h>

#define SECONDS_IN_A_DAY (86400UL)

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
        seconds_color = color32(0, 0, 255, 0);
        minutes_color = color32(0, 100, 150, 0);
        hours_color = color32(50, 50, 155, 0);
        monthday_color = color32(100, 0, 150, 0);
        weekday_color = color32(200, 200, 150, 0);
        break;

    case WARM:
        seconds_color = COLOR_MAROON; // pink
        minutes_color = COLOR_MAROON;
        hours_color = COLOR_MAROON;
        monthday_color = COLOR_MAROON;
        weekday_color = COLOR_MAROON;
        break;

    case EXCITED:
        seconds_color = COLOR_CRIMSON; // light pink
        minutes_color = COLOR_CRIMSON;
        hours_color = COLOR_CRIMSON;
        monthday_color = COLOR_CRIMSON;
        weekday_color = COLOR_CRIMSON;
        break;

    case MELLOW:
        // seconds_color = COLOR_BLUE;
        // minutes_color = COLOR_CHARTREUSE;
        // hours_color = COLOR_CYAN;
        // monthday_color = COLOR_AQUAMARINE;
        // weekday_color = COLOR_ORANGE;
        seconds_color = COLOR_MAROON;
        minutes_color = COLOR_CRIMSON;
        hours_color = color32(255, 100, 0, 0);
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

    case RAINBOW:
        seconds_color = COLOR_MAROON;
        minutes_color = COLOR_CRIMSON;
        hours_color = color32(255, 100, 0, 0);
        monthday_color = COLOR_AQUAMARINE;
        weekday_color = COLOR_ORANGE;
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
    uint32_t weekday_day = unix_timestamp / SECONDS_IN_A_DAY + 1;

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
    endPos = (STRIP_WEEKDAY_START > STRIP_WEEKDAY_END) ? (startPos - ((((weekday_day + 2) % 7) + 1) * 3))
                                                       : (startPos + ((((weekday_day + 2) % 7) + 1) * 3));
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

// 50 80 110 140
/*
    kevin 28-5-2064

*/
void strip_drawPensions(uint16_t distance_cm)
{
    char text[80];

    setStrip_clear();
    Strip_send();
    if (distance_cm > 0 && distance_cm <= 50) {
        lcd_set_cursor(0, 0);
        sprintf(text, "%s              ", e_developer_name[Kevin]);
        lcd_print(text);

        lcd_set_cursor(0, 1);
        sprintf(text, "%d/%d/%d             ", 28, 5, 2065);
        lcd_print(text);
    }

    if (distance_cm > 50 && distance_cm <= 80) {
        lcd_set_cursor(0, 0);
        sprintf(text, "%s              ", e_developer_name[Roel]);
        lcd_print(text);

        lcd_set_cursor(0, 1);
        sprintf(text, "%d/%d/%d             ", 30, 7, 2045);
        lcd_print(text);
    }

    if (distance_cm > 80 && distance_cm <= 110) {
        lcd_set_cursor(0, 0);
        sprintf(text, "%s              ", e_developer_name[Maarten]);
        lcd_print(text);

        lcd_set_cursor(0, 1);
        sprintf(text, "%d/%d/%d             ", 1, 1, 2065);
        lcd_print(text);
    }

    if (distance_cm > 110 && distance_cm <= 140) {
        lcd_set_cursor(0, 0);
        sprintf(text, "%s              ", e_developer_name[William]);
        lcd_print(text);

        lcd_set_cursor(0, 1);
        sprintf(text, "%d/%d/%d             ", 3, 1, 2070);
        lcd_print(text);
    }
}

void strip_drawTime_HUE(uint64_t unix_timestamp)
{
    uint16_t startPos = 0;
    uint16_t endPos = 0;
    uint32_t weekday_day = unix_timestamp / SECONDS_IN_A_DAY + 1;

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
    endPos = (STRIP_WEEKDAY_START > STRIP_WEEKDAY_END) ? (startPos - ((((weekday_day + 2) % 7) + 1) * 3))
                                                       : (startPos + ((((weekday_day + 2) % 7) + 1) * 3));
    for (uint16_t i = return_smaller(startPos, endPos); i < return_bigger(startPos, endPos); i++) {
        if (i < return_smaller(startPos, endPos) + 3 * 1) {
            setStrip_pixel(i, ColorHSV(0, 255, 255, 0));
        } else if (i < return_smaller(startPos, endPos) + 3 * 2) {
            setStrip_pixel(i, ColorHSV(10700, 255, 255, 0));
        } else if (i < return_smaller(startPos, endPos) + 3 * 3) {
            setStrip_pixel(i, ColorHSV(55000, 255, 255, 0));
        } else if (i < return_smaller(startPos, endPos) + 3 * 4) {
            setStrip_pixel(i, ColorHSV(32403, 255, 255, 0));
        } else if (i < return_smaller(startPos, endPos) + 3 * 5) {
            setStrip_pixel(i, ColorHSV(19660, 255, 255, 0));
        } else if (i < return_smaller(startPos, endPos) + 3 * 6) {
            setStrip_pixel(i, ColorHSV(42598, 255, 255, 0));
        } else if (i < return_smaller(startPos, endPos) + 3 * 7) {
            setStrip_pixel(i, ColorHSV(60000, 255, 255, 0));
        }
    }

    // setStrip_part(return_smaller(startPos, endPos), return_bigger(startPos, endPos), weekday_color);

    Strip_send();
}
