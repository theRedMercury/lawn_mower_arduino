/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#ifndef PERIMETER_H
#define PERIMETER_H

#include "../mower/abs_mower.hpp"

#define PIN_A_WIRE PIN_A5 // A5
#define RAW_SIGNAL_SAMPLE_SIZE 96
#define SENDER_ARRAY_SIZE 24 // 96
#define CORELLATION_ARRAY_SIZE (RAW_SIGNAL_SAMPLE_SIZE - SENDER_ARRAY_SIZE + 1)

class perimeter : public abs_mower_class
{
public:
    using abs_mower_class::abs_mower_class;
    void setup();
    void update();
    const int16_t get_magnitude() const;

    const int16_t get_smooth_magnitude() const;

    const float get_filter_quality() const;
    const bool is_inside() const;

    const boolean signal_timed_out() const;
    const String get_json() const;

private:
    bool _is_inside = false;
    int32_t _signal_counter, _magnitude, _smooth_magnitude = 0;
    unsigned long _last_inside_time = 0;
    float _filter_quality = 0.f;

    int16_t _correlation_signal[CORELLATION_ARRAY_SIZE];
    const int8_t _signal_code[SENDER_ARRAY_SIZE] = {1, 1, -1, -1, 1, -1, 1, -1, -1, 1, -1, 1, 1, -1, -1, 1, -1, -1, 1, -1, -1, 1, 1, -1};
};

#endif