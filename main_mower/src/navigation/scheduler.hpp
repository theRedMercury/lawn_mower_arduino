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
    unsigned char h_start = 0;
    unsigned char m_start = 0;
    unsigned char h_end = 0;
    unsigned char m_end = 0;
};

struct PACKING schedul_week
{
    schedul_day monday;
    schedul_day tuesday;
    schedul_day wednesday;
    schedul_day thursday;
    schedul_day friday;
    schedul_day saturday;
    schedul_day sunday;
};

class scheduler : public abs_mower_class
{
public:
    using abs_mower_class::abs_mower_class;
    void setup();
    void update();

    void force_mowing();
    void reset_force_mowing();
    bool is_time_to_mown() const;
    bool is_time_to_return_station() const;

    String get_string_schedul_day(const unsigned char day_week_num) const;
    const schedul_week *get_schedul() const;
    void set_scheduler(const unsigned char day_week_num, const unsigned char h_start, const unsigned char m_start, const unsigned char h_end, const unsigned char m_end);

private:
    bool _comp_hours(const unsigned char h_01, const unsigned char m_01, const unsigned char h_02, const unsigned char m_02) const;
    String _get_string_schedul_day(const schedul_day day) const;

    schedul_week _shedul;
    bool _force_to_mown = false; // until return station
};
#endif