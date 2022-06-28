/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "sonar.hpp"
#include "../mower/mower.hpp"

_sonar::_sonar(const unsigned char pin_trig, const unsigned char pin_echo)
{
    _pin_trig = pin_trig;
    _pin_echo = pin_echo;
    pinMode(_pin_trig, OUTPUT);
    digitalWrite(_pin_trig, LOW);
    pinMode(_pin_echo, INPUT);
}

void _sonar::update()
{
    digitalWrite(_pin_trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(_pin_trig, LOW);
    noInterrupts();
    _value = pulseIn(_pin_echo, HIGH, MEASURE_TIME_OUT) / 2.0f * SOUND_SPEED;
    interrupts();
}

unsigned short _sonar::get_value() const
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

unsigned short sonar_sensor::get_left() const
{
    return sonar_left.get_value();
}

unsigned short sonar_sensor::get_center() const
{
    return sonar_center.get_value();
}

unsigned short sonar_sensor::get_right() const
{
    return sonar_right.get_value();
}

void sonar_sensor::get_collisions(unsigned char *collision, const unsigned short threshold) const
{
    const unsigned char v0 = collision[0];
    const unsigned char v1 = collision[1];
    const unsigned char v2 = collision[2];
    collision[0] = (get_left() < threshold && get_left() != 0) ? constrain(v0 + 1, 0, 10) : 0;
    collision[1] = (get_center() < threshold && get_center() != 0) ? constrain(v1 + 1, 0, 10) : 0;
    collision[2] = (get_right() < threshold && get_right() != 0) ? constrain(v2 + 1, 0, 10) : 0;

    DEBUG_PRINT("COLLISION >");
    DEBUG_PRINT("\tLeft  : " + String(static_cast<int>(collision[0])) + " / " + String(get_left()));
    DEBUG_PRINT("\tCenter: " + String(static_cast<int>(collision[1])) + " / " + String(get_center()));
    DEBUG_PRINT("\tRight : " + String(static_cast<int>(collision[2])) + " / " + String(get_right()));
    DEBUG_PRINT("\tThreshold " + String(threshold));
    DEBUG_PRINTLN("");
}