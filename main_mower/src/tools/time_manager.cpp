/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include <Arduino.h>

#include "time_manager.hpp"

#define SECS_PER_MIN 60UL
#define SECS_PER_HOUR 3600UL
#define SECS_PER_DAY SECS_PER_HOUR * 24UL
#define DAYS_PER_WEEK 7UL
#define SECS_PER_WEEK SECS_PER_DAY *DAYS_PER_WEEK
#define SECS_PER_YEAR SECS_PER_DAY * 365UL // TODO: ought to handle leap years
#define SECS_YR_2000 946684800UL           // the time at the start of y2k
#define LEAP_YEAR(Y) (((1970 + (Y)) > 0) && !((1970 + (Y)) % 4) && (((1970 + (Y)) % 100) || !((1970 + (Y)) % 400)))

static constexpr char _month_str_0[] PROGMEM = "";
static constexpr char _month_str_1[] PROGMEM = "January";
static constexpr char _month_str_2[] PROGMEM = "February";
static constexpr char _month_str_3[] PROGMEM = "March";
static constexpr char _month_str_4[] PROGMEM = "April";
static constexpr char _month_str_5[] PROGMEM = "May";
static constexpr char _month_str_6[] PROGMEM = "June";
static constexpr char _month_str_7[] PROGMEM = "July";
static constexpr char _month_str_8[] PROGMEM = "August";
static constexpr char _month_str_9[] PROGMEM = "September";
static constexpr char _month_str_10[] PROGMEM = "October";
static constexpr char _month_str_11[] PROGMEM = "November";
static constexpr char _month_str_12[] PROGMEM = "December";

static constexpr PROGMEM const char *const PROGMEM _month_names[] = {
    _month_str_0, _month_str_1, _month_str_2, _month_str_3, _month_str_4, _month_str_5, _month_str_6,
    _month_str_7, _month_str_8, _month_str_9, _month_str_10, _month_str_11, _month_str_12};

static constexpr char _day_str_0[] PROGMEM = "Err";
static constexpr char _day_str_1[] PROGMEM = "Sun";
static constexpr char _day_str_2[] PROGMEM = "Mon";
static constexpr char _day_str_3[] PROGMEM = "Tues";
static constexpr char _day_str_4[] PROGMEM = "Wednes";
static constexpr char _day_str_5[] PROGMEM = "Thurs";
static constexpr char _day_str_6[] PROGMEM = "Fri";
static constexpr char _day_str_7[] PROGMEM = "Satur";

static constexpr const PROGMEM char *const PROGMEM _day_names[] = {
    _day_str_0, _day_str_1, _day_str_2, _day_str_3, _day_str_4, _day_str_5, _day_str_6, _day_str_7};

static constexpr const uint8_t _month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

time_manager::time_manager()
{
    set_time(1, 1, 1, 1, 1, 1970);
}
void time_manager::set_time(const uint8_t hr, const uint8_t minute, const uint8_t sec, const uint8_t dy, const uint8_t mnth, const uint16_t yr)
{
    // year can be given as full four digit year or two digts (2010 or 10 for 2010);
    // it is converted to years since 1970
    _current_tm.year_offset = (yr > 99) ? (yr - 1970) : (yr + 30);
    _current_tm.month = constrain(mnth, 1, 12);
    _current_tm.day = constrain(dy, 1, 31);
    _current_tm.hour = constrain(hr, 0, 24);
    _current_tm.minute = constrain(minute, 0, 60);
    _current_tm.second = constrain(sec, 0, 60);
    _make_time();
}

const uint8_t time_manager::get_second() const
{
    return _current_tm.second;
}
const uint8_t time_manager::get_minute() const
{
    return _current_tm.minute;
}
const uint8_t time_manager::get_hour() const
{
    return _current_tm.hour;
}
const uint8_t time_manager::get_day() const
{
    return _current_tm.day;
}
const time_day_week time_manager::get_day_week_num() const
{
    return _current_tm.wday;
}
const uint8_t time_manager::get_month() const
{
    return _current_tm.month;
}
const uint16_t time_manager::get_year() const
{
    return _current_tm.year_offset + 1970;
}

const unsigned long time_manager::get_current_time() const
{
    return _current_time;
}

const char *time_manager::get_current_day()
{
    strcpy_P(_buffer, (PGM_P)pgm_read_ptr(&(_day_names[static_cast<int>(_current_tm.wday)])));
    return _buffer;
}

const char *time_manager::get_current_month()
{
    strcpy_P(_buffer, (PGM_P)pgm_read_ptr(&(_month_names[_current_tm.month])));
    return _buffer;
}

void time_manager::adjust_time(const unsigned long adjustment)
{
    _current_time += adjustment;
    _update_time();
}

void time_manager::_update_time()
{
    // break the given time_t into time components
    // this is a more compact version of the C library localtime function
    // note that year is offset from 1970 !!!

    uint8_t year;
    uint8_t month, month_length;
    uint32_t new_time;
    unsigned long days;

    new_time = _current_time;
    _current_tm.second = new_time % 60;
    new_time /= 60; // now it is minutes
    _current_tm.minute = new_time % 60;
    new_time /= 60; // now it is hours
    _current_tm.hour = new_time % 24;
    new_time /= 24;                                                          // now it is days
    _current_tm.wday = static_cast<time_day_week>(((new_time + 4) % 7) + 1); // Sunday is day 1

    year = 0;
    days = 0;
    while ((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= new_time)
    {
        year++;
    }
    _current_tm.year_offset = year; // year is offset from 1970

    days -= LEAP_YEAR(year) ? 366 : 365;
    new_time -= days; // now it is days in this year, starting at 0

    days = 0;
    month = 0;
    month_length = 0;
    for (month = 0; month < 12; month++)
    {
        if (month == 1)
        { // february
            if (LEAP_YEAR(year))
            {
                month_length = 29;
            }
            else
            {
                month_length = 28;
            }
        }
        else
        {
            month_length = _month_days[month];
        }

        if (new_time >= month_length)
        {
            new_time -= month_length;
        }
        else
        {
            break;
        }
    }
    _current_tm.month = month + 1;  // jan is month 1
    _current_tm.day = new_time + 1; // day of month
}

void time_manager::_make_time()
{
    // assemble time elements into time_t
    // note year argument is offset from 1970 (see macros in time.h to convert to other formats)
    // previous version used full four digit year (or digits since 2000),i.e. 2009 was 2009 or 9

    int i;
    // seconds from 1970 till 1 jan 00:00:00 of the given year
    _current_time = _current_tm.year_offset * (SECS_PER_DAY * 365);
    for (i = 0; i < _current_tm.year_offset; i++)
    {
        if (LEAP_YEAR(i))
        {
            _current_time += SECS_PER_DAY; // add extra days for leap years
        }
    }

    // add days for this year, months start from 1
    for (i = 1; i < _current_tm.month; i++)
    {
        if ((i == 2) && LEAP_YEAR(_current_tm.year_offset))
        {
            _current_time += SECS_PER_DAY * 29;
        }
        else
        {
            _current_time += SECS_PER_DAY * _month_days[i - 1]; // monthDay array starts from 0
        }
    }
    _current_time += (_current_tm.day - 1) * SECS_PER_DAY;
    _current_time += _current_tm.hour * SECS_PER_HOUR;
    _current_time += _current_tm.minute * SECS_PER_MIN;
    _current_time += _current_tm.second;
    _update_time();
}