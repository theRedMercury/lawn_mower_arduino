/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "scheduler.hpp"
#include "../mower/mower.hpp"

#include <EEPROM.h>

void scheduler::setup()
{
    DEBUG_PRINT("SETUP : ");
    DEBUG_PRINT(class_name);
    EEPROM.get(EEADDR, _shedul);
    DEBUG_PRINTLN(" : DONE");
}

void scheduler::update()
{
    // useless
}

const bool scheduler::is_time_to_mown() const
{
#ifdef DEBUG_IS_TIME_TO_MOWN
    return true;
#endif
    if (!mower->gps.is_time_valid())
    {
        return false;
    }

    const uint8_t day_week_num = static_cast<uint8_t>(mower->time.get_day_week_num());
    const uint8_t h = mower->time.get_hour();
    const uint8_t m = mower->time.get_minute();
    bool b_start, b_end;
    switch (day_week_num)
    {
    case 1:
        b_start = _comp_hours(h, m, _shedul.sunday.h_start, _shedul.sunday.m_start);
        b_end = _comp_hours(h, m, _shedul.sunday.h_end, _shedul.sunday.m_end);
        return b_start && !b_end;
    case 2:
        b_start = _comp_hours(h, m, _shedul.monday.h_start, _shedul.monday.m_start);
        b_end = _comp_hours(h, m, _shedul.monday.h_end, _shedul.monday.m_end);
        return b_start && !b_end;

    case 3:
        b_start = _comp_hours(h, m, _shedul.tuesday.h_start, _shedul.tuesday.m_start);
        b_end = _comp_hours(h, m, _shedul.tuesday.h_end, _shedul.tuesday.m_end);
        return b_start && !b_end;

    case 4:
        b_start = _comp_hours(h, m, _shedul.wednesday.h_start, _shedul.wednesday.m_start);
        b_end = _comp_hours(h, m, _shedul.wednesday.h_end, _shedul.wednesday.m_end);
        return b_start && !b_end;

    case 5:
        b_start = _comp_hours(h, m, _shedul.thursday.h_start, _shedul.thursday.m_start);
        b_end = _comp_hours(h, m, _shedul.thursday.h_end, _shedul.thursday.m_end);
        return b_start && !b_end;

    case 6:
        b_start = _comp_hours(h, m, _shedul.friday.h_start, _shedul.friday.m_start);
        b_end = _comp_hours(h, m, _shedul.friday.h_end, _shedul.friday.m_end);
        return b_start && !b_end;

    case 7:
        b_start = _comp_hours(h, m, _shedul.saturday.h_start, _shedul.saturday.m_start);
        b_end = _comp_hours(h, m, _shedul.saturday.h_end, _shedul.saturday.m_end);
        return b_start && !b_end;

    default:
        break;
    }
    return false;
}

const bool scheduler::is_time_to_return_station() const
{
#ifdef DEBUG_IS_TIME_TO_MOWN
    return false;
#endif

    if (!mower->gps.is_time_valid())
    {
        return false;
    }

    const uint8_t day_week_num = static_cast<uint8_t>(mower->time.get_day_week_num());
    const uint8_t h = mower->time.get_hour();
    const uint8_t m = mower->time.get_minute();

    switch (day_week_num)
    {
    case 1:
        return _comp_hours(h, m, _shedul.sunday.h_end, _shedul.sunday.m_end);
    case 2:
        return _comp_hours(h, m, _shedul.monday.h_end, _shedul.monday.m_end);

    case 3:
        return _comp_hours(h, m, _shedul.tuesday.h_end, _shedul.tuesday.m_end);

    case 4:
        return _comp_hours(h, m, _shedul.wednesday.h_end, _shedul.wednesday.m_end);

    case 5:
        return _comp_hours(h, m, _shedul.thursday.h_end, _shedul.thursday.m_end);

    case 6:
        return _comp_hours(h, m, _shedul.friday.h_end, _shedul.friday.m_end);

    case 7:
        return _comp_hours(h, m, _shedul.saturday.h_end, _shedul.saturday.m_end);

    default:
        break;
    }
    return true;
}

const schedul_week *scheduler::get_schedul() const
{
    return &_shedul;
}

const String scheduler::get_string_schedul_day(const uint8_t day_week_num) const
{
    switch (day_week_num)
    {
    case 1:
        return _get_string_schedul_day(_shedul.monday);
        break;
    case 2:
        return _get_string_schedul_day(_shedul.tuesday);
        break;
    case 3:
        return _get_string_schedul_day(_shedul.wednesday);
        break;
    case 4:
        return _get_string_schedul_day(_shedul.thursday);
        break;
    case 5:
        return _get_string_schedul_day(_shedul.friday);
        break;
    case 6:
        return _get_string_schedul_day(_shedul.saturday);
        break;
    case 7:
        return _get_string_schedul_day(_shedul.sunday);
        break;
    default:
        return "";
        break;
    }
}

void scheduler::set_scheduler(const uint8_t day_week_num, const uint8_t h_start, const uint8_t m_start, const uint8_t h_end, const uint8_t m_end)
{
    // 1 = Monday -> 7 = Sunday
    switch (day_week_num)
    {
    case 1:
        _shedul.monday.h_start = h_start;
        _shedul.monday.m_start = m_start;
        _shedul.monday.h_end = h_end;
        _shedul.monday.m_end = m_end;
        break;
    case 2:
        _shedul.tuesday.h_start = h_start;
        _shedul.tuesday.m_start = m_start;
        _shedul.tuesday.h_end = h_end;
        _shedul.tuesday.m_end = m_end;
        break;
    case 3:
        _shedul.wednesday.h_start = h_start;
        _shedul.wednesday.m_start = m_start;
        _shedul.wednesday.h_end = h_end;
        _shedul.wednesday.m_end = m_end;
        break;
    case 4:
        _shedul.thursday.h_start = h_start;
        _shedul.thursday.m_start = m_start;
        _shedul.thursday.h_end = h_end;
        _shedul.thursday.m_end = m_end;
        break;
    case 5:
        _shedul.friday.h_start = h_start;
        _shedul.friday.m_start = m_start;
        _shedul.friday.h_end = h_end;
        _shedul.friday.m_end = m_end;
        break;
    case 6:
        _shedul.saturday.h_start = h_start;
        _shedul.saturday.m_start = m_start;
        _shedul.saturday.h_end = h_end;
        _shedul.saturday.m_end = m_end;
        break;
    case 7:
        _shedul.sunday.h_start = h_start;
        _shedul.sunday.m_start = m_start;
        _shedul.sunday.h_end = h_end;
        _shedul.sunday.m_end = m_end;
        break;
    default:
        DEBUG_PRINTLN("set_scheduler fail");
        break;
    }
    EEPROM.put(EEADDR, _shedul);
    DEBUG_PRINTLN("scheduler save");
}

const bool scheduler::_comp_hours(const uint8_t h_01, const uint8_t m_01, const uint8_t h_02, const uint8_t m_02) const
{
    return ((h_01 * 100) + m_01) >= ((h_02 * 100) + m_02);
}

const String scheduler::_get_string_schedul_day(const schedul_day day) const
{
    String ret = "";
    if (day.h_start < 10)
    {
        ret += "0" + String(day.h_start);
    }
    else
    {
        ret += String(day.h_start);
    }
    ret += ":";
    if (day.m_start < 10)
    {
        ret += "0" + String(day.m_start);
    }
    else
    {
        ret += String(day.m_start);
    }

    ret += " > ";
    if (day.h_end < 10)
    {
        ret += "0" + String(day.h_end);
    }
    else
    {
        ret += String(day.h_end);
    }
    ret += ":";
    if (day.m_end < 10)
    {
        ret += "0" + String(day.m_end);
    }
    else
    {
        ret += String(day.m_end);
    }
    return ret;
}