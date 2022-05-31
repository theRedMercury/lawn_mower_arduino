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

enum class time_day_week : unsigned char
{
    Err = 0,
    Monday = 1,
    Tuesday = 2,
    Wednesday = 3,
    Thursday = 4,
    Friday = 5,
    Saturday = 6,
    Sunday = 7,
};

struct PACKING time_element
{
    unsigned char second;
    unsigned char minute;
    unsigned char hour;
    time_day_week wday; // day of week, monday is day 1
    unsigned char week_num;
    unsigned char day;
    unsigned char month;
    unsigned char year_offset; // offset from 1970;
};

class time_manager
{
public:
    time_manager();
    time_manager(const time_manager &) = delete;
    time_manager(time_manager &&) = delete;
    time_manager &operator=(const time_manager &) = delete;
    time_manager &operator=(time_manager &&) = delete;

    void set_time(const unsigned char hr, const unsigned char min, const unsigned char sec, const unsigned char dy, const unsigned char mnth, const unsigned short yr);
    unsigned char get_second() const;
    unsigned char get_minute() const;
    unsigned char get_hour() const;
    unsigned char get_day() const;
    time_day_week get_day_week_num() const;
    unsigned char get_month() const;
    unsigned short get_year() const;
    unsigned long get_current_time() const;

    // time_element *get_current_time() const;
    char *get_current_day();
    char *get_current_month();

private:
    void _make_time();
    void _compute_week_number();

    time_element _current_tm;
    unsigned long _current_time = 0;
    char _buffer[10];
};
#endif