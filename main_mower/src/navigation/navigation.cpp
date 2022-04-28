/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "navigation.hpp"
#include "../mower/mower.hpp"

#define SPEED_MAX 255
#define SPEED_REVERSE 90
#define SPEED_ROTATION 75
#define TOLERANCE_TARGET 8

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

        // Safety lost mower
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
    case navigation_pattern::TURN_TO_TARGET:
        return "TURN_TARG";
    case navigation_pattern::FIND_EXIT:
        return "FIND_EXIT";
    case navigation_pattern::FULL_REVERSE:
        return "FULL_REVERSE";
    case navigation_pattern::CIRCLE:
        return "CIRCLE";
    case navigation_pattern::FOLLOW_WIRE:
        return "WIRE";
    case navigation_pattern::LEAVING_STATION:
        return "LEAVING_STATION";
    case navigation_pattern::ENTRING_STATION:
        return "ENTRING_STATION";
    default:
        return "UNKNOW";
    }
}
navigation::navigation_pattern navigation::get_nav_pattern()
{
    return _nav_patter;
}

void navigation::start_mowing(const bool wait_target, const unsigned short target)
{
    if (target != 1000)
    {
        _set_target_angle(target);
    }
    else
    {
        update_target_angle();
    }

    _nav_patter = wait_target ? navigation_pattern::TURN_TO_TARGET : navigation_pattern::KEEP_TARGET;
    _nav_exit = navigation_exit::BYPASS;
    _delay_next_pattern.reset_delay(MAX_DELAY_PATTERN_MS);
    _delay_exit_pattern.reset_delay(MAX_DELAY_PATTERN_EXIT_MS);

    if (!mower->perim.is_inside())
    {
        _nav_patter = navigation_pattern::STOP;
    }
}

void navigation::update_target_angle()
{
    _set_target_angle(mower->gps.get_heading_deg());
}

const unsigned short navigation::get_target() const
{
    return _target_angle;
}

void navigation::_set_target_angle(const unsigned short target_angle)
{
    _target_angle = target_angle % 360;
}

void navigation::_update()
{
    const navigation_pattern pattern_senor = _get_pattern_sensor();
    DEBUG_PRINTLN("NAVI SENSOR > " + String(get_current_pattern_str(pattern_senor)));

    // Detect if mower is stuck when forward
    if ((_nav_patter == navigation_pattern::FIND_EXIT || _nav_patter == navigation_pattern::KEEP_TARGET) && _delay_next_pattern.is_time_out(true, 3000) && !mower->gyro.is_moving())
    {
        DEBUG_PRINTLN("NAVI > Mower Stuck");
        _nav_patter = navigation_pattern::FULL_REVERSE;
        _delay_exit_pattern.reset_delay(3500); // time reverse
        _process_pattern();
        return;
    }

    if (_nav_patter == navigation_pattern::FULL_REVERSE && _delay_next_pattern.is_time_out(true, 3500) && !mower->gyro.is_moving())
    {
        DEBUG_PRINTLN("NAVI > Mower Stuck go mowing");
        start_mowing();
        return;
    }

    if (_nav_patter == navigation_pattern::TURN_TO_TARGET && _delay_next_pattern.is_time_out(true, 3500))
    {
        start_mowing();
        return;
    }

    if (_nav_patter != navigation_pattern::FIND_EXIT && pattern_senor == navigation_pattern::FIND_EXIT)
    {
        _nav_patter = navigation_pattern::FIND_EXIT;
        _nav_exit = navigation_exit::BYPASS;
        _delay_next_pattern.reset_delay();
        _delay_exit_pattern.reset_delay();
    }

    if (_nav_patter != navigation_pattern::FIND_EXIT && pattern_senor == navigation_pattern::FULL_REVERSE)
    {
        _nav_patter = navigation_pattern::FULL_REVERSE;
        _nav_exit = navigation_exit::BYPASS;
        _delay_next_pattern.reset_delay();
        _delay_exit_pattern.reset_delay();
    }

    if (_nav_patter == navigation_pattern::FIND_EXIT && pattern_senor == navigation_pattern::KEEP_TARGET && _delay_exit_pattern.is_time_out()) // Exit found
    {
        update_target_angle();
        _nav_patter = navigation_pattern::KEEP_TARGET;
        _delay_next_pattern.reset_delay();
    }

    if (_nav_patter == navigation_pattern::BYPASS && pattern_senor == navigation_pattern::KEEP_TARGET)
    {
        _nav_patter = navigation_pattern::KEEP_TARGET;
        _delay_next_pattern.reset_delay();
    }

    if (_nav_patter == navigation_pattern::CIRCLE)
    {
        if (pattern_senor == navigation_pattern::KEEP_TARGET)
        {
            if (_circle_milli < millis())
            {
                //_circle_milli = millis() + (100 * (_delay_next_pattern + 1));
                _set_target_angle(_target_angle += 1);
            }
            /*if (_delay_next_pattern >= MAX_CIRCLE_DELAY_COUNTER)
            {
                _nav_patter = navigation_pattern::KEEP_TARGET; // Exit Circle Mode
            }*/
        }
        else
        {
            _nav_patter = navigation_pattern::FIND_EXIT;
            _nav_exit = navigation_exit::BYPASS;
            _delay_next_pattern.reset_delay();
            _delay_exit_pattern.reset_delay();
        }
    }

    // FINAL CONDITION (outside perimeter)
    if (!mower->perim.is_inside() && _nav_patter != navigation_pattern::EXIT_WIRE_DETECT)
    {
        _nav_patter = navigation_pattern::EXIT_WIRE_DETECT;
        _delay_next_pattern.reset_delay();
    }
    if (_nav_patter == navigation_pattern::EXIT_WIRE_DETECT && _delay_next_pattern.is_time_out(true, 5000))
    {
        _nav_patter = navigation_pattern::KEEP_TARGET;
        _delay_next_pattern.reset_delay();
    }

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
        _pattern_keep_target(); //_pattern_keep_target();
        break;

    case navigation_pattern::TURN_TO_TARGET:
        _pattern_turn_target(); //_pattern_keep_target();
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
        _pattern_keep_target(true, 160);
        break;

    default:
        break;
    }
}

const navigation::navigation_pattern navigation::_get_pattern_sensor()
{
    const float speed = static_cast<float>(abs(mower->motor.get_speed_left()) + abs(mower->motor.get_speed_right())) / 2.4f;
    mower->sonar.get_collisions(_collision, constrain(speed, 75.f, 220.f));

    if (!mower->perim.is_inside())
    {
        return navigation_pattern::FULL_REVERSE;
    }
    if (_collision[0] >= MIN_COLLITION_DETECT || _collision[1] >= MIN_COLLITION_DETECT || _collision[2] >= MIN_COLLITION_DETECT)
    {
        return navigation_pattern::FIND_EXIT;
    }
    return navigation_pattern::KEEP_TARGET;
}

void navigation::_pattern_find_exit()
{
    if (_delay_exit_pattern.is_time_out())
    {
        if (_collision[0] == 0 && _collision[1] == 0 && _collision[2] == 0)
        {
            start_mowing();
            return;
        }
        start_mowing(false, _target_angle + 35);
        return;
    }

    if (_collision[0] >= MIN_COLLITION_DETECT && _collision[1] >= MIN_COLLITION_DETECT && _collision[2] >= MIN_COLLITION_DETECT)
    {
        _delay_exit_pattern.reset_delay(2500);
        _pattern_full_reverse();
        return;
    }

    if ((_collision[0] >= MIN_COLLITION_DETECT || _collision[1] >= MIN_COLLITION_DETECT) && _collision[2] == 0)
    {
        _nav_exit = navigation_exit::TURN_LEFT;
        mower->motor.set(SPEED_ROTATION, -SPEED_ROTATION);
        _delay_exit_pattern.reset_delay(MAX_DELAY_PATTERN_EXIT_MS);
        return;
    }

    if (_collision[0] == 0 && _collision[1] == 0 && _collision[2] >= MIN_COLLITION_DETECT)
    {
        _nav_exit = navigation_exit::TURN_RIGHT;
        mower->motor.set(-SPEED_ROTATION, SPEED_ROTATION);
        _delay_exit_pattern.reset_delay(MAX_DELAY_PATTERN_EXIT_MS);
        return;
    }
}

void navigation::_pattern_full_reverse()
{
    if (_collision[0] == 0 && _collision[1] == 0 && _collision[2] == 0 && _delay_exit_pattern.is_time_out())
    {
        start_mowing(true, mower->gps.get_heading_deg() - 45);
        return;
    }

    _nav_exit = navigation_exit::FULL_REVERSE;
    mower->motor.set(-SPEED_REVERSE, -SPEED_REVERSE);
}

void navigation::_pattern_return_in_perim()
{
    if (_delay_exit_pattern.is_time_out())
    {
        start_mowing(true, mower->gps.get_heading_deg() - 45);
        return;
    }
    _nav_exit = navigation_exit::FULL_REVERSE;
    mower->motor.set(-SPEED_REVERSE, -SPEED_REVERSE);
    /*if (_delay_next_pattern < 10)
    {
        mower->motor.set(-SPEED_REVERSE, -SPEED_REVERSE);
    }
    if (_delay_next_pattern > 10)
    {
        mower->motor.set(SPEED_REVERSE, -SPEED_REVERSE);
    }
    if (_delay_next_pattern > 20)
    {
        mower->motor.set(0, 0);
    }*/
}

void navigation::_pattern_keep_target(bool high_keeping = false, unsigned char max_speed)
{
    const unsigned short current_angle = mower->gps.get_heading_deg();
    short diff_angle = current_angle - _target_angle;
    if (abs(diff_angle) > 180)
    {
        diff_angle = 360 - abs(diff_angle);
    }

    correction = map(abs(diff_angle), 0, 180, 0, 255);
    DEBUG_PRINTLN("ANGLE : T " + String(_target_angle) + " <> " + String(current_angle) + " >> " + String(correction) + " // " + String(diff_angle));

    if (high_keeping)
    {
        const short speed_nominal = constrain(max_speed - correction, 0, max_speed);
        const short speed_corr = constrain(max_speed - correction, -max_speed, max_speed);

        if (diff_angle < 90)
        {
            mower->motor.set(max_speed, 0);
            return;
        }

        if (diff_angle > 90)
        {
            mower->motor.set(0, max_speed);
            return;
        }

        if (diff_angle > TOLERANCE_TARGET)
        {
            mower->motor.set(speed_nominal, speed_corr);
            return;
        }
        if (diff_angle < -TOLERANCE_TARGET)
        {
            mower->motor.set(speed_corr, speed_nominal);

            return;
        }
        mower->motor.set(max_speed, max_speed);
        return;
    }
    else
    {
        if (diff_angle > 0)
        {
            mower->motor.set(max_speed, max_speed - correction);
            return;
        }
        if (diff_angle < 0)
        {
            mower->motor.set(max_speed - correction, max_speed);
            return;
        }
        mower->motor.set(max_speed, max_speed);
    }
}

void navigation::_pattern_turn_target()
{
    const unsigned short current_angle = mower->gps.get_heading_deg();
    short diff_angle = current_angle - _target_angle;
    if (abs(diff_angle) > 180)
    {
        diff_angle = 360 - abs(diff_angle);
    }

    if (diff_angle > TOLERANCE_TARGET + 8)
    {
        mower->motor.set(SPEED_ROTATION, -SPEED_ROTATION);
        return;
    }
    if (diff_angle < -(TOLERANCE_TARGET + 8))
    {
        mower->motor.set(-SPEED_ROTATION, SPEED_ROTATION);
        return;
    }
    start_mowing();
}

void navigation::_pattern_follow_wire()
{
    // Find wire
    if (mower->perim.is_inside() && !_wire_find)
    {
        _pattern_keep_target(true, 160);
        return;
    }
    // Wire found
    if (!mower->perim.is_inside() && !_wire_find)
    {
        _wire_find = true;
        mower->motor.set(-SPEED_REVERSE, -SPEED_REVERSE);
        _set_target_angle(_target_angle -= 35);
        _delay_next_pattern.reset_delay(1200);
        return;
    }

    if (_wire_find)
    {
        if (mower->perim.is_inside())
        {
            _pattern_keep_target(true, 120);
            return;
        }
        if (!mower->perim.is_inside() && _delay_next_pattern.is_time_out())
        {
            _pattern_keep_target(true, 120);
            _set_target_angle(_target_angle -= 10);
            _delay_next_pattern.reset_delay(250);
        }
    }
}

void navigation::_pattern_leaving_station()
{
    if (!_delay_next_pattern.is_time_out())
    {
        mower->motor.set(-SPEED_REVERSE, -SPEED_REVERSE);
        return;
    }

    start_mowing(true, mower->gps.get_heading_deg() - 45);
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
            mower->set_current_status(mower_status::LEAVING_STATION);
            _delay_next_pattern.reset_delay(3000);
            _delay_exit_pattern.reset_delay();
        }
        else
        {
            mower->set_current_status(mower_status::READY);
        }
    }
}
