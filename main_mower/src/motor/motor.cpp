/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "motor.hpp"
#include "../mower/mower.hpp"

void motor_control::setup()
{
    DEBUG_PRINT("SETUP : ");
    DEBUG_PRINT(class_name);
    _motor_left.PIN_FORWARD = PIN_MOTOR_LEFT_FORWARD;
    _motor_left.PIN_BACKWARD = PIN_MOTOR_LEFT_BACKWARD;
    _motor_left.PIN_SPEED = PIN_MOTOR_LEFT_SPEED;
    pinMode(_motor_left.PIN_FORWARD, OUTPUT);
    pinMode(_motor_left.PIN_BACKWARD, OUTPUT);
    pinMode(_motor_left.PIN_SPEED, OUTPUT);

    _motor_right.PIN_FORWARD = PIN_MOTOR_RIGHT_FORWARD;
    _motor_right.PIN_BACKWARD = PIN_MOTOR_RIGHT_BACKWARD;
    _motor_right.PIN_SPEED = PIN_MOTOR_RIGHT_SPEED;
    pinMode(_motor_right.PIN_FORWARD, OUTPUT);
    pinMode(_motor_right.PIN_BACKWARD, OUTPUT);
    pinMode(_motor_right.PIN_SPEED, OUTPUT);

    stop();
    DEBUG_PRINTLN(" : DONE");
}

void motor_control::update()
{
    if (!mower->gyro.in_safe_status())
    {
        DEBUG_PRINTLN("Motor: OFF Safety");
        mower->set_error(mower_status::ERROR_NOT_SAFE);
        return;
    }

    _update_motor_speed(&_motor_left);
    _update_motor_speed(&_motor_right);
    /*else
    {
        mower->set_error();
    }*/
}

void motor_control::stop()
{
    if (!_motor_left.is_stop || !_motor_right.is_stop)
    {
        _security_count = 0;
        _stop_motor(&_motor_left);
        _stop_motor(&_motor_right);
    }
}

void motor_control::set(short left_speed, short right_speed)
{
    left_speed = constrain(left_speed, -MOTOR_MAX_SPEED, MOTOR_MAX_SPEED);
    right_speed = constrain(right_speed, -MOTOR_MAX_SPEED, MOTOR_MAX_SPEED);

    if (abs(left_speed) > 90 || abs(right_speed) > 90)
    {
        _security_count++;
    }
    DEBUG_PRINTLN("set_motor :" + String(left_speed) + " / " + String(right_speed));

    // From zero to new value left motor
    if (_motor_left.target_speed == 0 && left_speed != 0)
    {
        _motor_left.new_speed_from_0.reset_delay();
        _motor_left.last_set_speed_time = millis() + 3000;
        _motor_left.target_speed = left_speed > 0 ? MOTOR_MAX_SPEED : -MOTOR_MAX_SPEED;
    }
    else
    {
        if (_motor_left.new_speed_from_0.is_time_out())
        {
            _motor_left.target_speed = left_speed;
        }
    }

    // For right motor
    if (_motor_right.target_speed == 0 && right_speed != 0)
    {
        _motor_right.new_speed_from_0.reset_delay();
        _motor_right.last_set_speed_time = millis() + 3000;
        _motor_right.target_speed = right_speed > 0 ? MOTOR_MAX_SPEED : -MOTOR_MAX_SPEED;
    }
    else
    {
        if (_motor_right.new_speed_from_0.is_time_out())
        {
            _motor_right.target_speed = left_speed;
        }
    }
}

void motor_control::clear_speed()
{
    DEBUG_PRINTLN("motor_control::clear_speed");
    _motor_left.computed_speed = 0.f;
    _motor_right.computed_speed = 0.f;
}

bool motor_control::is_running() const
{
    return get_speed_left() != 0 || get_speed_right() != 0;
}

short motor_control::get_speed_left() const
{
    if (_motor_left.target_speed < 0)
    {
        return -static_cast<short>(_motor_left.real_speed);
    }
    return static_cast<short>(_motor_left.real_speed);
}
short motor_control::get_speed_right() const
{
    if (_motor_right.target_speed < 0)
    {
        return -static_cast<short>(_motor_right.real_speed);
    }
    return static_cast<short>(_motor_right.real_speed);
}

void motor_control::_stop_motor(motor_stuct *motor)
{
    analogWrite(motor->PIN_SPEED, 0);
    digitalWrite(motor->PIN_FORWARD, LOW);
    digitalWrite(motor->PIN_BACKWARD, LOW);
    motor->computed_speed = 0;
    motor->target_speed = 0;
    motor->real_speed = 0;
    motor->is_stop = true;
}

void motor_control::_update_motor_speed(motor_stuct *motor)
{
    constexpr float accel_tac = 800.0F;                          // 2000 is Accel (not change < 2000)
    unsigned long TaC = (millis() - motor->last_set_speed_time); // sampling time in millis
    motor->last_set_speed_time = millis();
    if (TaC > 1000)
    {
        TaC = 1;
    }

    motor->computed_speed += static_cast<float>(TaC) * (static_cast<float>(motor->target_speed) - motor->computed_speed) / accel_tac;

    DEBUG_PRINTLN("computed_speed : " + String(motor->computed_speed) + " / " + String(motor->target_speed));

    motor->real_speed = constrain(abs(round(motor->computed_speed)),
                                  0,
                                  mower->perim.is_inside() ? MOTOR_MAX_SPEED : MOTOR_MAX_SPEED_OUTSIDE);
    /*switch (real_m)
    {
    case 0 ... 20:
        motor->real_speed = 0;
        break;
    case 21 ... 120:
        motor->real_speed = 120;
        break;
    case 244 ... 255:
        motor->real_speed = 255;
        break;
    default:
        motor->real_speed = real_m;
        break;
    }*/

    switch (motor->target_speed)
    {
    case -255 ... - 1:
        motor->is_stop = false;
        digitalWrite(motor->PIN_FORWARD, LOW);
        digitalWrite(motor->PIN_BACKWARD, HIGH);
        break;
    case 1 ... 255:
        motor->is_stop = false;
        digitalWrite(motor->PIN_FORWARD, HIGH);
        digitalWrite(motor->PIN_BACKWARD, LOW);
        break;
    default:
        digitalWrite(motor->PIN_FORWARD, LOW);
        digitalWrite(motor->PIN_BACKWARD, LOW);
        break;
    }

    analogWrite(motor->PIN_SPEED, motor->real_speed);
}