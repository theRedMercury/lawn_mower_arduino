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

enum class time_day_week : char
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
    unsigned char second;
    unsigned char minute;
    unsigned char hour;
    time_day_week wday; // day of week, sunday is day 1
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
    const unsigned char get_second() const;
    const unsigned char get_minute() const;
    const unsigned char get_hour() const;
    const unsigned char get_day() const;
    const time_day_week get_day_week_num() const;
    const unsigned char get_month() const;
    const unsigned short get_year() const;
    const unsigned long get_current_time() const;

    // const time_element *get_current_time() const;
    const char *get_current_day();
    const char *get_current_month();
    void adjust_time(const unsigned long adjustment);

private:
    void _make_time();
    void _update_time();

    time_element _current_tm;
    unsigned int _current_time = 0;
    char _buffer[10];
};
#endif