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
#include "timer_dma_ws2812.h"
#include "unixFunction.h"

static datetime_t dateTime;

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
    default:
        strip_drawTimeSimple(unix_timestamp);
        break;
    }
}

void strip_drawTimeSimple(uint32_t unix_timestamp)
{
    uint16_t startPos = 0;
    uint16_t endPos = 0;

    RTC_HAL_ConvertSecsToDatetime(&unix_timestamp, &dateTime);

    // draw seconds
    startPos = STRIP_SECONDS_START;
    endPos = STRIP_SECONDS_START + dateTime.second;
    if (endPos > STRIP_SECONDS_END) {
        endPos = STRIP_SECONDS_END;
    }
    setStrip_part(startPos, endPos, SECONDS_COLOR);

    // draw minutes
    startPos = STRIP_MINUTES_START;
    endPos = STRIP_MINUTES_START + dateTime.minute;
    if (endPos > STRIP_MINUTES_END) {
        endPos = STRIP_MINUTES_END;
    }
    setStrip_part(startPos, endPos, MINUTES_COLOR);

    // draw hours
    startPos = STRIP_HOURS_START;
    endPos = STRIP_HOURS_START + dateTime.hour;
    if (endPos > STRIP_HOURS_END) {
        endPos = STRIP_HOURS_END;
    }
    setStrip_part(startPos, endPos, HOURS_COLOR);

    // draw monthday
    startPos = STRIP_MONTHDAY_START;
    endPos = STRIP_MONTHDAY_START + (dateTime.day % 31);
    if (endPos > STRIP_MONTHDAY_END) {
        endPos = STRIP_MONTHDAY_END;
    }
    setStrip_part(startPos, endPos, MONTHDAY_COLOR);

    // draw weekday MONTHDAY TODO: fix this
    startPos = STRIP_WEEKDAY_START;
    endPos = STRIP_WEEKDAY_START + dateTime.day % 7;
    if (endPos > STRIP_WEEKDAY_END) {
        endPos = STRIP_WEEKDAY_END;
    }
    setStrip_part(startPos, endPos, WEEKDAY_COLOR);

    Strip_send();
}

void strip_drawTimeCool(uint32_t unix_timestamp) {}

void strip_drawTimeWarm(uint32_t unix_timestamp) {}

void strip_drawTimeExcited(uint32_t unix_timestamp) {}

void strip_drawTimeMellow(uint32_t unix_timestamp) {}

void strip_drawTimeChill(uint32_t unix_timestamp) {}

void strip_drawUltrasoneDistance(uint16_t distance_cm) {}

void strip_drawPensions(enum e_developer person, uint16_t distance_cm){}