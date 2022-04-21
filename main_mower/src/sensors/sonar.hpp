/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#ifndef SONAR_H
#define SONAR_H

#include "../mower/abs_mower.hpp"

#define PIN_USOUND_TRIG_01 40
#define PIN_USOUND_ECHO_01 38

#define PIN_USOUND_TRIG_02 36
#define PIN_USOUND_ECHO_02 34

#define PIN_USOUND_TRIG_03 32
#define PIN_USOUND_ECHO_03 30

#define MEASURE_TIME_OUT 10000UL // 10ms = ~3.4m à 340m/s
#define SOUND_SPEED 0.34029f     // Speed sound in mm/us

class _sensor
{
public:
    _sensor(const byte pin_trig, const byte pin_echo);
    void update();
    const float get_value() const;

private:
    float _value = 0.f;
    byte _pin_trig, _pin_echo;

    _sensor() = delete;
    _sensor(const _sensor &) = delete;
    _sensor(_sensor &&) = delete;
    _sensor &operator=(const _sensor &) = delete;
    _sensor &operator=(_sensor &&) = delete;
};

class sonar_sensor : public abs_mower_class
{
public:
    using abs_mower_class::abs_mower_class;
    void setup();
    void update();

    const float get_left() const;
    const float get_center() const;
    const float get_right() const;

    const void get_collisions(bool *collision, const float threshold = 50.f) const;

private:
    _sensor sonar_left{PIN_USOUND_TRIG_03, PIN_USOUND_ECHO_03};
    _sensor sonar_center{PIN_USOUND_TRIG_02, PIN_USOUND_ECHO_02};
    _sensor sonar_right{PIN_USOUND_TRIG_01, PIN_USOUND_ECHO_01};
};

#endif