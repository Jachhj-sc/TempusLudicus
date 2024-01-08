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

void strip_drawTimeMood(uint32_t unix_timestamp, enum e_mood mood)
{
    switch (mood) {
    case COOL:
        break;

    case WARM:
        break;

    case EXCITED:
        break;

    case MELLOW:
        break;

    case CHILL:
        break;

    case NORMAL:
        strip_drawTimeSimple(unix_timestamp);
        break;

    default:

        break;
    }
}

void strip_drawTimeSimple(uint32_t unix_timestamp)
{
    uint16_t startPos = 0;
    uint16_t endPos = 0;

    RTC_HAL_ConvertSecsToDatetime(&unix_timestamp, &dateTime);
    setStrip_clear();
	
    // draw seconds
    startPos = STRIP_SECONDS_START;
    endPos = (STRIP_SECONDS_START > STRIP_SECONDS_END) ? (startPos - dateTime.second) : (startPos + dateTime.second);
    setStrip_part(return_smaller(startPos, endPos), return_bigger(startPos, endPos), SECONDS_COLOR);

    // draw minutes
    startPos = STRIP_MINUTES_START;
    endPos = (STRIP_MINUTES_START > STRIP_SECONDS_END) ? (startPos - dateTime.minute) : (startPos + dateTime.minute);
    setStrip_part(return_smaller(startPos, endPos), return_bigger(startPos, endPos), MINUTES_COLOR);

    // draw hours
    startPos = STRIP_HOURS_START;
    endPos = (STRIP_HOURS_START > STRIP_HOURS_END) ? (startPos - dateTime.hour) : (startPos + dateTime.hour);
    setStrip_part(return_smaller(startPos, endPos), return_bigger(startPos, endPos), HOURS_COLOR);

    // draw monthday
    startPos = STRIP_MONTHDAY_START;
    endPos = (STRIP_MONTHDAY_START > STRIP_MONTHDAY_END) ? (startPos - dateTime.day) : (startPos + dateTime.day);
    setStrip_part(return_smaller(startPos, endPos), return_bigger(startPos, endPos), MONTHDAY_COLOR);

    // draw weekday MONTHDAY TODO: fix this
    startPos = STRIP_WEEKDAY_START;
    endPos = (STRIP_WEEKDAY_START > STRIP_WEEKDAY_END) ? (startPos - ((dateTime.day % 7) * 3))
                                                       : (startPos + ((dateTime.day % 7) * 3));
    setStrip_part(return_smaller(startPos, endPos), return_bigger(startPos, endPos), WEEKDAY_COLOR);

    Strip_send();
}

void strip_drawTimeCool(uint32_t unix_timestamp) {}

void strip_drawTimeWarm(uint32_t unix_timestamp) {}

void strip_drawTimeExcited(uint32_t unix_timestamp) {}

void strip_drawTimeMellow(uint32_t unix_timestamp) {}

void strip_drawTimeChill(uint32_t unix_timestamp) {}

void strip_drawUltrasoneDistance(uint16_t distance_cm) {}

void strip_drawPensions(enum e_developer person, uint16_t distance_cm) {}
