/**
 *  @brief  time manager
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <Arduino.h>
#include <stdint.h>

#include "tools.hpp"

enum class time_day_week
{
    Err,
    Sunday,
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday
};

struct PACKING time_element
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    time_day_week wday; // day of week, sunday is day 1
    uint8_t day;
    uint8_t month;
    uint8_t year_offset; // offset from 1970;
};

class time_manager
{
public:
    time_manager();
    time_manager(const time_manager &) = delete;
    time_manager(time_manager &&) = delete;
    time_manager &operator=(const time_manager &) = delete;
    time_manager &operator=(time_manager &&) = delete;

    void set_time(const uint8_t hr, const uint8_t min, const uint8_t sec, const uint8_t dy, const uint8_t mnth, const uint16_t yr);
    const uint8_t get_second() const;
    const uint8_t get_minute() const;
    const uint8_t get_hour() const;
    const uint8_t get_day() const;
    const time_day_week get_day_week_num() const;
    const uint8_t get_month() const;
    const uint16_t get_year() const;
    const unsigned long get_current_time() const;

    // const time_element *get_current_time() const;
    const char *get_current_day();
    const char *get_current_month();
    void adjust_time(const unsigned long adjustment);

private:
    void _make_time();
    void _update_time();

    time_element _current_tm;
    uint32_t _current_time = 0;
    char _buffer[10];
};
#endif