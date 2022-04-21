/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "sonar.hpp"
#include "../mower/mower.hpp"

_sensor::_sensor(const byte pin_trig, const byte pin_echo)
{
    _pin_trig = pin_trig;
    _pin_echo = pin_echo;
    pinMode(_pin_trig, OUTPUT);
    digitalWrite(_pin_trig, LOW);
    pinMode(_pin_echo, INPUT);
}
void _sensor::update()
{
    digitalWrite(_pin_trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(_pin_trig, LOW);
    noInterrupts();
    _value = pulseIn(_pin_echo, HIGH, MEASURE_TIME_OUT) / 2.0f * SOUND_SPEED;
    interrupts();
}
const float _sensor::get_value() const
{
    return _value;
}

void sonar_sensor::setup()
{
    DEBUG_PRINT("SETUP : ");
    DEBUG_PRINT(class_name);
    DEBUG_PRINTLN(" : DONE");
}

void sonar_sensor::update()
{
    sonar_left.update();
    sonar_center.update();
    sonar_right.update();

    DEBUG_PRINT("SONAR >");
    DEBUG_PRINT("\tLeft : " + String(sonar_left.get_value()));
    DEBUG_PRINT("\tCenter: " + String(sonar_center.get_value()));
    DEBUG_PRINT("\tRight: " + String(sonar_right.get_value()));
    DEBUG_PRINTLN("");
}

const float sonar_sensor::get_left() const
{
    return sonar_left.get_value();
}
const float sonar_sensor::get_center() const
{
    return sonar_center.get_value();
}
const float sonar_sensor::get_right() const
{
    return sonar_right.get_value();
}

const void sonar_sensor::get_collisions(bool *collision, const float threshold) const
{
    collision[0] = get_left() < threshold && get_left() != 0;
    collision[1] = get_center() < threshold && get_center() != 0;
    collision[2] = get_right() < threshold && get_right() != 0;

    DEBUG_PRINT("COLLISION >");
    DEBUG_PRINT("\tLeft : " + String(collision[0]));
    DEBUG_PRINT("\tCenter: " + String(collision[1]));
    DEBUG_PRINT("\tRight: " + String(collision[2]));
    DEBUG_PRINTLN("");
}