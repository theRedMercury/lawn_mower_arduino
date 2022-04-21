/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../mower/abs_mower.hpp"
#include "../tools/time_manager.hpp"
#include "../tools/tools.hpp"

#define EEADDR 0 // Start location to write EEPROM data.

struct PACKING schedul_day
{
    uint8_t h_start = 0;
    uint8_t m_start = 0;
    uint8_t h_end = 0;
    uint8_t m_end = 0;
};

struct PACKING schedul_week
{
    schedul_day sunday;
    schedul_day monday;
    schedul_day tuesday;
    schedul_day wednesday;
    schedul_day thursday;
    schedul_day friday;
    schedul_day saturday;
};

class scheduler : public abs_mower_class
{
public:
    using abs_mower_class::abs_mower_class;
    void setup();
    void update();

    const bool is_time_to_mown() const;
    const bool is_time_to_return_station() const;

    const String get_string_schedul_day(const uint8_t day_week_num) const;
    const schedul_week *get_schedul() const;
    void set_scheduler(const uint8_t day_week_num, const uint8_t h_start, const uint8_t m_start, const uint8_t h_end, const uint8_t m_end);

private:
    const bool _comp_hours(const uint8_t h_01, const uint8_t m_01, const uint8_t h_02, const uint8_t m_02) const;
    const String _get_string_schedul_day(const schedul_day day) const;

    schedul_week _shedul;
    bool _force_to_mown = false;
};
#endif