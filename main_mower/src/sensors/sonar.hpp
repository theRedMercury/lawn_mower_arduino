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

class _sonar
{
public:
    _sonar(const unsigned char pin_trig, const unsigned char pin_echo);
    void update();
    float get_value() const;

private:
    float _value = 0.f;
    unsigned char _pin_trig, _pin_echo;

    _sonar() = delete;
    _sonar(const _sonar &) = delete;
    _sonar(_sonar &&) = delete;
    _sonar &operator=(const _sonar &) = delete;
    _sonar &operator=(_sonar &&) = delete;
};

class sonar_sensor : public abs_mower_class
{
public:
    using abs_mower_class::abs_mower_class;
    void setup();
    void update();

    float get_left() const;
    float get_center() const;
    float get_right() const;

    void get_collisions(unsigned char *collision, const float threshold = 50.f) const;

private:
    _sonar sonar_left{PIN_USOUND_TRIG_03, PIN_USOUND_ECHO_03};
    _sonar sonar_center{PIN_USOUND_TRIG_02, PIN_USOUND_ECHO_02};
    _sonar sonar_right{PIN_USOUND_TRIG_01, PIN_USOUND_ECHO_01};
};

#endif