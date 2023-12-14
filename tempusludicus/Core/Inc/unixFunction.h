#ifndef UNIXFUNCTION_H
#define UNIXFUNCTION_H

#include <MKL25Z4.h>
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

void RTC_HAL_ConvertSecsToDatetime(const volatile uint32_t *seconds, datetime_t *datetime);
void RTC_HAL_ConvertDatetimeToSecs(const datetime_t *datetime, uint32_t *seconds);

#endif
