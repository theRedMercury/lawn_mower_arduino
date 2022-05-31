/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "motor_blade.hpp"
#include "../mower/mower.hpp"

void motor_control_blade::setup()
{
    DEBUG_PRINT("SETUP : ");
    DEBUG_PRINT(class_name);
    pinMode(PIN_L_EN, OUTPUT);
    pinMode(PIN_R_EN, OUTPUT);
    pinMode(PIN_RPWM, OUTPUT);
    pinMode(PIN_RELAY, OUTPUT);
    digitalWrite(PIN_RELAY, HIGH); // OFF relay
    stop();
    DEBUG_PRINTLN(" : DONE");
}

void motor_control_blade::update()
{
    if (!mower->gyro.in_safe_status())
    {
        DEBUG_PRINTLN("Blades: OFF Safety");
        mower->set_error();
        return;
    }

    if (_blade_on && _speed <= MAX_RPWW_SPEED)
    {
        _speed += 4; // Linear acceleration
        analogWrite(PIN_RPWM, constrain(_speed, 0, MAX_RPWW_SPEED));
    }
}

void motor_control_blade::stop()
{
    if (_blade_on)
    {
        _speed = 0;
        digitalWrite(PIN_RELAY, HIGH);
        digitalWrite(PIN_L_EN, LOW);
        digitalWrite(PIN_R_EN, LOW);
        analogWrite(PIN_RPWM, 0);
        DEBUG_PRINTLN("Blades: 0FF");
        _blade_on = false;
    }
}

void motor_control_blade::on()
{
    if (!_blade_on)
    {
        digitalWrite(PIN_RELAY, LOW);
        digitalWrite(PIN_L_EN, HIGH);
        digitalWrite(PIN_R_EN, HIGH);
        _speed = 12;
        DEBUG_PRINTLN("Blades: ON");
        _blade_on = true;
    }
}

bool motor_control_blade::is_on() const
{
    return _blade_on;
}
