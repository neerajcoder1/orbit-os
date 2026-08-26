#ifndef RTC_H
#define RTC_H

#include <stdint.h>

struct rtc_time {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
};

void rtc_initialize(void);
void rtc_get_time(struct rtc_time* time);

#endif
