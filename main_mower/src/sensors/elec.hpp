/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#ifndef ELEC_H
#define ELEC_H

#include "../mower/abs_mower.hpp"

#define PIN_A_VOLT PIN_A1 // A1
#define PIN_A_AMP PIN_A0  // A0
#define AMP_CHARGING 0.6f
#define BATERRY_MIN 10.2f
#define BATERRY_MAX 13.0f
#define BARERRY_TIME_OUT 255 // 8 sec+-

class elec_sensor : public abs_mower_class
{
public:
    using abs_mower_class::abs_mower_class;
    void setup();
    void update();

    void update_volt();

    void update_amp();

    const bool is_battery_charged() const;
    const bool is_battery_low() const;

    const bool is_charging() const;
    const float get_curent_volt() const;
    const float get_curent_amp() const;

    const String get_json() const;

private:
    float _current_volt = -1;
    float _current_amp = -1.f;
    uint16_t _cumulation_volt_min_time = 0;
    uint16_t _cumulation_volt_max_time = 0;
    uint16_t _cumulation_charge_max_time = 0;

    /*unsigned long _previous_millis = 0;      // Track time in milliseconds since last reading
    const unsigned long _read_period = 1000; // in milliseconds
    const float _intercept = 0.f; // to be adjusted based on calibration testing
    const float _slope = 0.0752f; // to be adjusted based on calibration testing
    RunningStatistics _input_stats;*/
};

#endif