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

static constexpr char _day_str_0[] PROGMEM = "Err   ";
static constexpr char _day_str_1[] PROGMEM = "Mon   ";
static constexpr char _day_str_2[] PROGMEM = "Tues  ";
static constexpr char _day_str_3[] PROGMEM = "Wednes";
static constexpr char _day_str_4[] PROGMEM = "Thurs ";
static constexpr char _day_str_5[] PROGMEM = "Fri   ";
static constexpr char _day_str_6[] PROGMEM = "Satur ";
static constexpr char _day_str_7[] PROGMEM = "Sun   ";

static constexpr const PROGMEM char *const PROGMEM _day_names[] = {
    _day_str_0, _day_str_1, _day_str_2, _day_str_3, _day_str_4, _day_str_5, _day_str_6, _day_str_7};

static constexpr const unsigned char _month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

time_manager::time_manager()
{
    set_time(1, 1, 1, 1, 1, 1970);
}

void time_manager::set_time(const unsigned char hr, const unsigned char minute, const unsigned char sec, const unsigned char dy, const unsigned char mnth, const unsigned short yr)
{
    _current_tm.hour = constrain(hr, 0, 23);
    _current_tm.minute = constrain(minute, 0, 59);
    _current_tm.second = constrain(sec, 0, 59);
    // year can be given as full four digit year or two digts (2010 or 10 for 2010);
    // it is converted to years since 1970
    _current_tm.day = constrain(dy, 1, 31);
    _current_tm.month = constrain(mnth, 1, 12);
    _current_tm.year_offset = (yr > 99) ? (yr - 1970) : (yr + 30);

    _make_time();
}

unsigned char time_manager::get_second() const
{
    return _current_tm.second;
}

unsigned char time_manager::get_minute() const
{
    return _current_tm.minute;
}

unsigned char time_manager::get_hour() const
{
    return _current_tm.hour;
}

unsigned char time_manager::get_day() const
{
    return _current_tm.day;
}

time_day_week time_manager::get_day_week_num() const
{
    return _current_tm.wday;
}

unsigned char time_manager::get_month() const
{
    return _current_tm.month;
}

unsigned short time_manager::get_year() const
{
    return _current_tm.year_offset + 1970;
}

unsigned long time_manager::get_current_time() const
{
    return _current_time;
}

char *time_manager::get_current_day()
{
    strcpy_P(_buffer, (PGM_P)pgm_read_ptr(&(_day_names[static_cast<int>(_current_tm.wday)])));
    return _buffer;
}

char *time_manager::get_current_month()
{
    strcpy_P(_buffer, (PGM_P)pgm_read_ptr(&(_month_names[_current_tm.month])));
    return _buffer;
}

void time_manager::_compute_week_number()
{
    int d = _current_tm.day;
    int m = _current_tm.month;
    int y = _current_tm.year_offset + 1970;
    // take advantage of 28-year cycle
    while (y >= 1929)
        y -= 28;
    // compute adjustment for dates within the year
    //     If Jan. 1 falls on: Mo Tu We Th Fr Sa Su
    // then the adjustment is:  6  7  8  9  3  4  5
    int adj = (y - 1873) + ((y - 1873) >> 2);
    while (adj > 9)
        adj -= 7;
    // compute day of the year (in range 1-366)
    int doy = d;
    for (int i = 1; i < m; i++)
        doy += (30 + ((0x15AA >> i) & 1));
    if (m > 2)
        doy -= ((y & 3) ? 2 : 1);
    // compute the adjusted day number
    int dnum = adj + doy;
    // compute week number
    int wknum = dnum >> 3;
    dnum -= ((wknum << 3) - wknum);
    while (dnum >= 7)
    {
        dnum -= 7;
        wknum++;
    }
    // check for boundary conditions
    if (wknum < 1)
    {
        // last week of the previous year
        // check to see whether that year had 52 or 53 weeks
        // re-compute adjustment, this time for previous year
        adj = (y - 1874) + ((y - 1874) >> 2);
        while (adj > 9)
            adj -= 7;
        // all years beginning on Thursday have 53 weeks
        if (adj == 9)
        {
            _current_tm.week_num = 53;
            return;
        }
        // leap years beginning on Wednesday have 53 weeks
        if ((adj == 8) && ((y & 3) == 1))
        {
            _current_tm.week_num = 53;
            return;
        }
        // other years have 52 weeks
        _current_tm.week_num = 52;
        return;
    }
    if (wknum > 52)
    {
        // check to see whether week 53 exists in this year
        // all years beginning on Thursday have 53 weeks
        if (adj == 9)
        {
            _current_tm.week_num = 53;
            return;
        }
        // leap years beginning on Wednesday have 53 weeks
        if ((adj == 8) && ((y & 3) == 0))
        {
            _current_tm.week_num = 53;
            return;
        }
        // other years have 52 weeks
        _current_tm.week_num = 1;
        return;
    }
    _current_tm.week_num = wknum;
}

void time_manager::_make_time()
{
    // assemble time elements into time_t
    // note year argument is offset from 1970 (see macros in time.h to convert to other formats)
    // previous version used full four digit year (or digits since 2000),i.e. 2009 was 2009 or 9

    _compute_week_number();
    // seconds from 1970 till 1 jan 00:00:00 of the given year
    _current_time = _current_tm.year_offset * (SECS_PER_DAY * 365);
    for (int i = 0; i < _current_tm.year_offset; i++)
    {
        if (LEAP_YEAR(i))
        {
            _current_time += SECS_PER_DAY; // add extra days for leap years
        }
    }

    // add days for this year, months start from 1
    for (int i = 1; i < _current_tm.month; i++)
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
    int day_week = (((_current_time / 86400) + 4) % 7);
    day_week = day_week < 1 ? 7 : day_week; // Fix monday / sunday
    day_week = day_week > 7 ? 1 : day_week;
    _current_tm.wday = static_cast<time_day_week>(day_week); // Monday is 1
}