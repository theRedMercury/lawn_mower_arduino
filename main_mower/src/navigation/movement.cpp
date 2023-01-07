/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "movement.hpp"
#include "../mower/mower.hpp"

void movement::setup()
{
    DEBUG_PRINT("SETUP : ");
    DEBUG_PRINT(class_name);
    DEBUG_PRINTLN(" : DONE");
}

void movement::update()
{
    _apply_current_mouvement();

    /*
     * AY = up/down
     * AX = left/right
     */

    float speed = abs(_max_speed * ((mower->gyro.get_AY() * 1.4f) - 1.0f));
    float r_speed = abs(140 * ((mower->gyro.get_AY() * 1.4f) - 1.0f));

    switch (_current_movement)
    {
    case movement_process::FORWARD:
        // Adapt speed with Sonar value
        if (mower->sonar.get_center() > 0 && mower->sonar.get_center() < 500)
        {
            speed *= constrain((float)((float)mower->sonar.get_center() / 500.f), 0.3f, 1.f);
        }
        else
        {
            if (mower->sonar.get_left() > 0 && mower->sonar.get_left() < 500)
            {
                speed *= constrain((float)((float)mower->sonar.get_left() / 500.f), 0.3f, 1.f);
            }
            else
            {
                if (mower->sonar.get_right() > 0 && mower->sonar.get_right() < 500)
                {
                    speed *= constrain((float)((float)mower->sonar.get_right() / 500.f), 0.3f, 1.f);
                }
            }
        }

        mower->motor.set(speed, speed);
        break;

    case movement_process::TURN_LEFT:
        mower->motor.set(-r_speed, speed);
        break;

    case movement_process::TURN_RIGHT:
        mower->motor.set(speed, -r_speed);
        break;

    case movement_process::FULL_REVERSE:
        mower->motor.set(-speed, -speed);
        break;

    case movement_process::WITH_CORRECTION:

        if (_correction > 0)
        {
            mower->motor.set(speed, speed - float(_correction));
            break;
        }
        if (_correction < 0)
        {
            mower->motor.set(speed - float(abs(_correction)), speed);
            break;
        }
        mower->motor.set(speed, speed);
        break;

    case movement_process::WAIT:
        mower->motor.set(0, 0);
        break;

    default:
        break;
    }
}

void movement::stop()
{
    _current_movement == movement_process::WAIT;
    _list_index = 0;
    _list_inde_proced = 0;
    _delay_current_movement.reset_delay(1);
}

void movement::_apply_current_mouvement()
{
    if (_list_index > 0)
    {
        if (_delay_current_movement.is_time_out())
        {
            _delay_current_movement.reset_delay(_list_movement[_list_inde_proced].delay);
            _current_movement = _list_movement[_list_inde_proced].movement;
            _current_delay = _list_movement[_list_inde_proced].delay;
            _max_speed = _list_movement[_list_inde_proced].max_speed;
            _list_inde_proced++;

            // End task
            if (_list_inde_proced > _list_index)
            {
                _list_index = 0;
                _list_inde_proced = 0;
                _current_movement = movement_process::WAIT;
                _delay_current_movement.reset_delay(1);
            }
        }
        return;
    }

    if (_current_movement != _next_movement)
    {

        if (_current_delay > 0 && _delay_current_movement.is_time_out())
        {
            if (_next_movement != movement_process::WITH_CORRECTION)
            {
                mower->motor.clear_speed();
            }
            _delay_current_movement.reset_delay(_next_delay);
            _current_movement = _next_movement;
            _current_delay = _next_delay;
        }
        if (_current_delay < 0)
        {
            _current_movement = _next_movement;
            _current_delay = _next_delay;
        }
    }

    if (_current_delay > 0 && _delay_current_movement.is_time_out())
    {
        if (_next_movement != movement_process::WITH_CORRECTION)
        {
            mower->motor.clear_speed();
        }
        _current_movement = movement_process::WAIT;
    }

    DEBUG_PRINTLN("MOVEMENT Current : " + String(get_current_pattern_str(_current_movement)));
    DEBUG_PRINTLN("MOVEMENT Delay : " + String(_current_delay));
}

const char movement::get_current_pattern_str(const movement_process n) const
{
    switch (n)
    {
    case movement_process::WAIT:
        return 'W';
    case movement_process::FORWARD:
        return 'F';
    case movement_process::TURN_LEFT:
        return 'L';
    case movement_process::TURN_RIGHT:
        return 'R';
    case movement_process::FULL_REVERSE:
        return 'B';
    case movement_process::WITH_CORRECTION:
        return 'C';
    default:
        return '?';
    }
}

bool movement::not_task() const
{
    return _list_index == 0 && _list_inde_proced == 0;
}

bool movement::is_waiting() const
{
    return ((_current_movement == movement_process::WAIT && not_task()) ||
            _current_movement == movement_process::WITH_CORRECTION);
}

void movement::forward(bool stack_list, short max_delay_ms, unsigned short max_speed)
{
    if (stack_list)
    {
        _push_movement(movement_process::FORWARD, max_delay_ms, max_speed);
        return;
    }

    if (_current_movement == movement_process::WAIT)
    {
        _next_movement = movement_process::FORWARD;
        _next_delay = max_delay_ms;
        _max_speed = max_speed;
    }
}

void movement::turn_left(bool stack_list, short max_delay_ms, unsigned short max_speed)
{
    if (stack_list)
    {
        _push_movement(movement_process::TURN_LEFT, max_delay_ms, max_speed);
        return;
    }

    if (_current_movement == movement_process::WAIT || _current_movement == movement_process::FORWARD)
    {
        if (_current_movement == movement_process::TURN_LEFT)
        {
            return;
        }
        _next_movement = movement_process::TURN_LEFT;
        _next_delay = max_delay_ms;
        _max_speed = max_speed;
    }
}

void movement::turn_right(bool stack_list, short max_delay_ms, unsigned short max_speed)
{
    if (stack_list)
    {
        _push_movement(movement_process::TURN_RIGHT, max_delay_ms, max_speed);
        return;
    }

    if (_current_movement == movement_process::WAIT || _current_movement == movement_process::FORWARD)
    {
        if (_current_movement == movement_process::TURN_RIGHT)
        {
            return;
        }
        _next_movement = movement_process::TURN_RIGHT;
        _next_delay = max_delay_ms;
        _max_speed = max_speed;
    }
}

void movement::reverse(bool stack_list, short max_delay_ms, unsigned short max_speed)
{
    if (stack_list)
    {
        _push_movement(movement_process::FULL_REVERSE, max_delay_ms, max_speed);
        return;
    }

    if (_current_movement == movement_process::WAIT || _current_movement == movement_process::FORWARD)
    {
        if (_current_movement == movement_process::FULL_REVERSE)
        {
            return;
        }
        _next_movement = movement_process::FULL_REVERSE;
        _next_delay = max_delay_ms;
        _max_speed = max_speed;
    }
}

void movement::with_correction(short correction, short max_delay_ms, unsigned short max_speed)
{
    _next_movement = movement_process::WITH_CORRECTION;
    _current_delay = -1;
    _next_delay = max_delay_ms;
    _max_speed = max_speed;
    _correction = correction;
}

void movement::_push_movement(movement_process mov, short delay, unsigned short max_speed)
{
    // Can't queue without delay
    if (_list_index < SIZE_LIST_MOVEMENT && delay > 0)
    {
        // If first push -> reset delay
        if (_list_index == 0)
        {
            _delay_current_movement.reset_delay(1);
        }
        movement_action a = {};
        a.movement = mov;
        a.delay = delay;
        a.max_speed = max_speed;
        _list_movement[_list_index] = a;
        _list_index++;
    }
}