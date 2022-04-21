/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "navigation.hpp"
#include "../mower/mower.hpp"

void navigation::setup()
{
    DEBUG_PRINT("SETUP : ");
    DEBUG_PRINT(class_name);
    DEBUG_PRINTLN(" : DONE");
}
void navigation::update()
{
    DEBUG_PRINTLN("NAVI  > " + String(get_current_pattern_str(_nav_patter)) + " > " + String(static_cast<int>(_nav_exit)));
    // MAIN NAVIGATION SWITCH
    switch (mower->get_current_status())
    {
    case mower_status::ERROR_INIT:
    case mower_status::ERROR_LOST_WIRE:
    case mower_status::ERROR:
    case mower_status::READY:
        // Do Nothing
        break;

    case mower_status::WAITING:
        mower->motor.stop();
        mower->motor_blade.stop();
        break;

    case mower_status::CHARGING:
        mower->motor.stop();
        mower->motor_blade.stop();
        _charching();
        break;

    case mower_status::RETURN_STATION:
        mower->motor_blade.stop();
        _nav_patter = navigation_pattern::FOLLOW_WIRE;
        _update();
        break;

    case mower_status::RUNNING:
        // Return station
        if (mower->schedul.is_time_to_return_station() || mower->elec.is_battery_low())
        {
            mower->motor_blade.stop();
            mower->set_current_status(mower_status::RETURN_STATION);
            return;
        }

        if (mower->perim.signal_timed_out())
        {
            mower->set_error(mower_status::ERROR_LOST_WIRE);
            return;
        }
        mower->motor_blade.on();
        _update();
        break;

    case mower_status::TESTING_NAV:
        _update();
        break;

    case mower_status::TESTING_NAV_02:
        _nav_patter = navigation_pattern::CIRCLE;
        _update();
        break;

    default:
        break;
    }
}

const char *navigation::get_current_pattern_str(const navigation_pattern n) const
{
    switch (n)
    {
    case navigation_pattern::BYPASS:
        return "BYPASS";
    case navigation_pattern::STOP:
        return "STOP";
    case navigation_pattern::KEEP_TARGET:
        return "KEEP_TARGET";
    case navigation_pattern::FIND_EXIT:
        return "FIND_EXIT";
    case navigation_pattern::CIRCLE:
        return "CIRCLE";
    case navigation_pattern::FOLLOW_WIRE:
        return "WIRE";
    default:
        return "UNKNOW";
    }
}

void navigation::update_target_angle()
{
    _target_angle = mower->gps.get_heading_deg();
}

void navigation::_update()
{
    const navigation_pattern pattern_senor = _get_pattern_sensor();

    // Safety
    if ((mower->get_current_status() != mower_status::RUNNING || mower->get_current_status() < mower_status::TESTING_NAV) && !mower->perim.is_inside() && mower->motor.is_running())
    {
        mower->motor_blade.stop();
        mower->motor.stop();
        return;
    }

    // Detect if mower is stuck
    if ((_nav_patter == navigation_pattern::FIND_EXIT || _nav_patter == navigation_pattern::KEEP_TARGET) && _delay_next_pattern >= MAX_DELAY_COUNTER && !mower->gyro.is_moving())
    {
        _nav_patter = navigation_pattern::FULL_REVERSE;
    }

    if (_nav_patter != navigation_pattern::FIND_EXIT && pattern_senor == navigation_pattern::FIND_EXIT)
    {
        _nav_patter = navigation_pattern::FIND_EXIT;
        _nav_exit = navigation_exit::BYPASS;
        _delay_next_pattern = 0;
        _delay_exit_pattern = 0;
    }
    if (_nav_patter == navigation_pattern::FIND_EXIT && pattern_senor == navigation_pattern::KEEP_TARGET && _delay_next_pattern > MAX_DELAY_COUNTER) // Exit found
    {
        update_target_angle();
        _nav_patter = navigation_pattern::KEEP_TARGET;
        _delay_next_pattern = 0;
    }

    if (_nav_patter == navigation_pattern::BYPASS && pattern_senor == navigation_pattern::KEEP_TARGET)
    {
        _nav_patter = navigation_pattern::KEEP_TARGET;
        _delay_next_pattern = 0;
    }

    if (_nav_patter == navigation_pattern::CIRCLE)
    {
        if (pattern_senor == navigation_pattern::KEEP_TARGET)
        {
            if (_circle_milli < millis())
            {
                _circle_milli = millis() + (100 * (_delay_next_pattern + 1));
                _target_angle += 1;
            }
            if (_delay_next_pattern >= MAX_CIRCLE_DELAY_COUNTER)
            {
                _nav_patter = navigation_pattern::KEEP_TARGET; // Exit Circle Mode
            }
        }
        else
        {
            _nav_patter = navigation_pattern::FIND_EXIT;
            _delay_next_pattern = 0;
        }
    }

    // FINAL CONDITION (outside perimeter)
    if (!mower->perim.is_inside() && _nav_patter != navigation_pattern::EXIT_WIRE_DETECT)
    {
        _nav_patter = navigation_pattern::EXIT_WIRE_DETECT;
        _delay_next_pattern = 0;
    }
    if (_nav_patter == navigation_pattern::EXIT_WIRE_DETECT && _delay_next_pattern > (MAX_DELAY_COUNTER * 2))
    {
        _nav_patter = navigation_pattern::KEEP_TARGET;
        _delay_next_pattern = 0;
    }

    _delay_next_pattern++;
    _process_pattern();
}

void navigation::_process_pattern()
{
    switch (_nav_patter)
    {
    case navigation_pattern::BYPASS:
        return;
        break;

    case navigation_pattern::FIND_EXIT:
        _pattern_find_exit();
        break;

    case navigation_pattern::EXIT_WIRE_DETECT:
        _pattern_return_in_perim();
        break;

    case navigation_pattern::KEEP_TARGET:
        _pattern_keep_target();
        break;

    case navigation_pattern::FOLLOW_WIRE:
        _pattern_follow_wire();
        break;

    case navigation_pattern::FULL_REVERSE:
        _pattern_full_reverse();
        break;

    case navigation_pattern::LEAVING_STATION:
        _pattern_leaving_station();
        break;

    case navigation_pattern::ENTRING_STATION:
        _pattern_entering_station();
        break;

    case navigation_pattern::STOP:
        mower->motor.stop();
        mower->motor_blade.stop();
        break;

    case navigation_pattern::CIRCLE:
        _pattern_keep_target();
        break;

    default:
        break;
    }
}

const navigation::navigation_pattern navigation::_get_pattern_sensor()
{
    float speed = float(mower->motor.get_speed_left() + mower->motor.get_speed_right()) / 2.f;
    mower->sonar.get_collisions(_collision, constrain(speed, 175.f, 250.f));
    if (_collision[0] || _collision[1] || _collision[2])
    {
        return navigation_pattern::FIND_EXIT;
    }
    return navigation_pattern::KEEP_TARGET;
}

void navigation::_pattern_find_exit()
{
    _delay_exit_pattern++;
    if (!_collision[0] && !_collision[1] && !_collision[2])
    {
        _delay_exit_pattern = (MAX_DELAY_COUNTER - 2);
    }
    if (_nav_exit != navigation_exit::BYPASS && _delay_exit_pattern < MAX_DELAY_COUNTER)
    {
        return;
    }
    if (_collision[0] && _collision[1] && _collision[2])
    {
        _nav_exit = navigation_exit::FULL_REVERSE;
        mower->motor.set(-185, -185);
        return;
    }
    if (!_collision[0] && !_collision[1] && _collision[2])
    {
        _nav_exit = navigation_exit::TURN_RIGHT;
        mower->motor.set(-80, 80);
        return;
    }
    _nav_exit = navigation_exit::TURN_LEFT;
    mower->motor.set(80, -80);
}

void navigation::_pattern_full_reverse()
{
    _nav_exit = navigation_exit::FULL_REVERSE;
    mower->motor.set(-100, -100);
    return;
}

void navigation::_pattern_return_in_perim()
{
    if (_delay_next_pattern < 10)
    {
        mower->motor.set(-255, -255);
    }
    if (_delay_next_pattern > 10)
    {
        mower->motor.set(255, -255);
    }
    if (_delay_next_pattern > 20)
    {
        mower->motor.set(0, 0);
    }
}

void navigation::_pattern_keep_target(uint8_t max_speed)
{
    const float current_angle = mower->gps.get_heading_deg();
    const short correction = constrain(abs(round(_target_angle) - round(current_angle)), 0, 180) * 20;

    DEBUG_PRINTLN("ANGLE : T " + String(_target_angle) + " <> " + String(current_angle) + " >> " + String(correction));

    if ((current_angle + 2.f) < _target_angle)
    {
        mower->motor.set(255, 255 - correction);
        return;
    }
    if (current_angle > (_target_angle + 2.f))
    {
        mower->motor.set(255 - correction, 255);
        return;
    }
    mower->motor.set(255, 255);
}

void navigation::_pattern_follow_wire()
{
    // Find wire
    if (mower->perim.is_inside() && !_wire_find)
    {
        _pattern_keep_target();
        return;
    }
    // Wire found
    if (!mower->perim.is_inside() && !_wire_find)
    {
        _wire_find = true;
        mower->motor.set(-180, -180);
        _target_angle += 90;
    }

    if (mower->perim.is_inside() && _wire_find)
    {
        _pattern_keep_target();
    }
    if (!mower->perim.is_inside() && _wire_find)
    {
        _pattern_keep_target();
        _target_angle += 10;
    }
}

void navigation::_pattern_leaving_station()
{
}
void navigation::_pattern_entering_station()
{
}

void navigation::_charching()
{
    if (mower->elec.is_battery_charged())
    {
        if (mower->schedul.is_time_to_mown())
        {
            mower->set_current_status(mower_status::RUNNING);
        }
        else
        {
            mower->set_current_status(mower_status::READY);
        }
    }
}