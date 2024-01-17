#ifndef UNIXFUNCTION_H
#define UNIXFUNCTION_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint16_t year;
    uint16_t month;
    uint16_t day;
    uint16_t hour;
    uint16_t minute;
    uint16_t second;
} datetime_t;

void RTC_HAL_convert_unix_to_datetime(const uint64_t seconds, datetime_t *datetime);
void RTC_HAL_convert_datetime_to_unix(const datetime_t *datetime, uint64_t *seconds);

#endif
