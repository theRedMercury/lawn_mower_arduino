/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#ifndef PERIMETER_H
#define PERIMETER_H

#include "../mower/abs_mower.hpp"

#define PIN_A_WIRE PIN_A5       // A5
#define SIGNAL_TIME_OUT_MS 8000 // 8 sec
#define RAW_SIGNAL_SAMPLE_SIZE 96
#define SENDER_ARRAY_SIZE 24
#define CORELLATION_ARRAY_SIZE (RAW_SIGNAL_SAMPLE_SIZE - SENDER_ARRAY_SIZE + 1)

class perimeter : public abs_mower_class
{
public:
    using abs_mower_class::abs_mower_class;
    void setup();
    void update();

    void set_simulate_is_inside(const bool simulate = false);
    bool get_simulate_is_inside() const;

    short get_magnitude() const;
    short get_smooth_magnitude() const;
    float get_filter_quality() const;

    bool is_inside() const;
    bool is_signal_timed_out() const;

    String get_json() const;

private:
    bool _is_inside = false;
    bool _simulate_is_inside = false;
    int _signal_counter, _magnitude, _smooth_magnitude = 0;
    unsigned long _last_inside_time = 0;
    float _filter_quality = 0.f;

    short _correlation_signal[CORELLATION_ARRAY_SIZE];
    const char _signal_code[SENDER_ARRAY_SIZE] = {1, 1, -1, -1, 1, -1, 1, -1, -1, 1, -1, 1, 1, -1, -1, 1, -1, -1, 1, -1, -1, 1, 1, -1};
};

#endif