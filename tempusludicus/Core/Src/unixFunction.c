#include "unixFunction.h"

#define SECONDS_IN_A_DAY    (86400UL)
#define SECONDS_IN_A_HOUR   (3600UL)
#define SECONDS_IN_A_MIN    (60UL)
#define MINS_IN_A_HOUR      (60UL)
#define HOURS_IN_A_DAY      (24UL)
#define DAYS_IN_A_YEAR      (365UL)
#define DAYS_IN_A_LEAP_YEAR (366UL)
#define YEAR_RANGE_START    (1970UL)
#define YEAR_RANGE_END      (2099UL)
/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Table of month length (in days) for the Un-leap-year*/
static const uint8_t ULY[] = {0U, 31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};
/* Table of month length (in days) for the Leap-year*/
static const uint8_t LY[] = {0U, 31U, 29U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};
/* Number of days from begin of the non Leap-year*/
static const uint16_t MONTH_DAYS[] = {0U, 0U, 31U, 59U, 90U, 120U, 151U, 181U, 212U, 243U, 273U, 304U, 334U};

void RTC_HAL_convert_unix_to_datetime(const uint32_t seconds, datetime_t *datetime)
{
    uint32_t Seconds, Days, Days_in_year;
    const uint8_t *Days_in_month;
    /* Start from 1970-01-01*/
    Seconds = seconds;
    /* days, we add 1 for the current day which is represented in the hours and seconds field */
    Days = Seconds / SECONDS_IN_A_DAY + 1;
    /* seconds left*/
    Seconds = Seconds % SECONDS_IN_A_DAY;
    /* hours*/
    datetime->hour = (uint16_t)(Seconds / SECONDS_IN_A_HOUR);
    /* seconds left*/
    Seconds = Seconds % SECONDS_IN_A_HOUR;
    /* minutes*/
    datetime->minute = (uint16_t)(Seconds / SECONDS_IN_A_MIN);
    /* seconds*/
    datetime->second = Seconds % SECONDS_IN_A_MIN;
    /* year*/
    datetime->year = YEAR_RANGE_START;
    Days_in_year = DAYS_IN_A_YEAR;
    while (Days > Days_in_year) {
        Days -= Days_in_year;
        datetime->year++;
        if (datetime->year & 3U) {
            Days_in_year = DAYS_IN_A_YEAR;
        } else {
            Days_in_year = DAYS_IN_A_LEAP_YEAR;
        }
    }
    if (datetime->year & 3U) {
        Days_in_month = ULY;
    } else {
        Days_in_month = LY;
    }
    for (uint32_t x = 1U; x <= 12U; x++) {
        if (Days <= (*(Days_in_month + x))) {
            datetime->month = (uint16_t)x;
            break;
        } else {
            Days -= (*(Days_in_month + x));
        }
    }
    datetime->day = (uint16_t)Days;
}

void RTC_HAL_convert_datetime_to_unix(const datetime_t *datetime, uint32_t *seconds)
{
    /* Compute number of days from 1970 till given year*/
    *seconds = (datetime->year - 1970U) * DAYS_IN_A_YEAR;
    /* Add leap year days */
    *seconds += ((datetime->year / 4) - (1970U / 4));
    /* Add number of days till given month*/
    *seconds += MONTH_DAYS[datetime->month];
    /* Add days in given month. We take away seconds for the current day as it is
     * represented in the hours and seconds field*/
    *seconds += (datetime->day - 1);
    /* For leap year if month less than or equal to Febraury, decrement day counter*/
    if ((!(datetime->year & 3U)) && (datetime->month <= 2U)) {
        (*seconds)--;
    }
    *seconds = ((*seconds) * SECONDS_IN_A_DAY) + (datetime->hour * SECONDS_IN_A_HOUR) +
               (datetime->minute * SECONDS_IN_A_MIN) + datetime->second;
}
