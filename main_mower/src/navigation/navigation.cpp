/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "navigation.hpp"
#include "../mower/mower.hpp"

#define SPEED_MAX 255
#define SPEED_REVERSE 110
#define SPEED_ROTATION 100
#define TOLERANCE_TARGET 30

void navigation::setup()
{
    DEBUG_PRINT("SETUP : ");
    DEBUG_PRINT(class_name);
    DEBUG_PRINTLN(" : DONE");
}

void navigation::update()
{
    DEBUG_PRINTLN("NAVI  > " + String(get_current_pattern_str(_nav_patter)));
    _get_pattern_sensor();
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

        // Safety lost mower
        if (mower->perim.is_signal_timed_out())
        {
            mower->schedul.reset_force_mowing();
            mower->set_error(mower_status::ERROR_LOST_WIRE);
            return;
        }

        _update_navigation();
        break;

    case mower_status::RUNNING:
        // Return station
        if (mower->schedul.is_time_to_return_station() || mower->elec.is_battery_low() || mower->gyro.is_temp_warning())
        {
            mower->motor_blade.stop();
            mower->schedul.reset_force_mowing();
            mower->set_current_status(mower_status::RETURN_STATION);
            return;
        }
        // Temp critical
        if (mower->gyro.is_temp_critical())
        {
            mower->set_error(mower_status::ERROR_TEMP);
            return;
        }

        // Safety lost mower
        if (mower->perim.is_signal_timed_out())
        {
            mower->schedul.reset_force_mowing();
            mower->set_error(mower_status::ERROR_LOST_WIRE);
            return;
        }
        mower->motor_blade.on();

        // Wait blade motor running and stable ====
        if (!_delay_start_mowing_init.is_time_out())
        {
            return;
        }
        if (!_save_gyro_state)
        {
            _save_gyro_state = true;
            mower->gyro.stop_init_gyro();
            _nav_patter = navigation_pattern::KEEP_TARGET;
            _delay_next_pattern.reset_delay();
            _delay_exit_pattern.reset_delay();
            _delay_not_moving.reset_delay();
            _delay_not_moving_cri.reset_delay();
            _delay_start_mowing_init.reset_delay();
        }
        //==========================================

        _update_navigation();
        break;

    case mower_status::TESTING_NAV:
        _update_navigation();
        break;

    case mower_status::TESTING_NAV_02:
        _nav_patter = navigation_pattern::CIRCLE;
        _update_navigation();
        break;

    default:
        break;
    }
}

const char *navigation::get_current_pattern_str(const navigation_pattern n) const
{
    switch (n)
    {
    case navigation_pattern::STOP:
        return "STOP   ";
    case navigation_pattern::KEEP_TARGET:
        return "KEEP_TA";
    case navigation_pattern::TURN_TO_TARGET:
        return "TURN_TA";
    case navigation_pattern::FIND_EXIT:
        return "FIND_EX";
    case navigation_pattern::EXIT_WIRE_DETECT:
        return "EXIT_WI";
    case navigation_pattern::FULL_REVERSE:
        return "FULL_RE";
    case navigation_pattern::CIRCLE:
        return "CIRCLE ";
    case navigation_pattern::FOLLOW_WIRE:
        return "FOL_WIR";
    case navigation_pattern::LEAVING_STATION:
        return "LEAV_ST";
    case navigation_pattern::ENTRING_STATION:
        return "ENTR_ST";
    default:
        return "UNKNOW ";
    }
}
navigation::navigation_pattern navigation::get_nav_pattern()
{
    return _nav_patter;
}

void navigation::start_move()
{
    update_target_angle();
    _delay_not_moving.reset_delay();
    _delay_not_moving_cri.reset_delay();
    _delay_next_pattern.reset_delay();
}

void navigation::start_mowing()
{
    _delay_not_moving.reset_delay();
    _delay_not_moving_cri.reset_delay();
    _delay_start_mowing_init.reset_delay();
    _save_gyro_state = false;
    mower->gyro.start_init_gyro();
    _start_mowing();
}

void navigation::_start_mowing(const bool wait_target, const unsigned short target)
{
    if (mower->get_current_status() == mower_status::RETURN_STATION)
    {
        _nav_patter = navigation_pattern::FOLLOW_WIRE;
        return;
    }
    if (!mower->schedul.is_time_to_mown())
    {
        mower->set_current_status(mower_status::RETURN_STATION);
        return;
    }

    if (target != 1000) // default value
    {
        //_set_target_angle(target);
        update_target_angle();
    }
    else
    {
        update_target_angle();
    }

    _wire_find = false;
    _nav_patter = wait_target ? navigation_pattern::FIND_EXIT : navigation_pattern::KEEP_TARGET;
    _nav_patter = navigation_pattern::KEEP_TARGET;
    _delay_next_pattern.reset_delay();
    _delay_exit_pattern.reset_delay();

    if (!mower->perim.is_inside())
    {
        _nav_patter = navigation_pattern::STOP;
    }
}

void navigation::update_target_angle()
{
    _set_target_angle(mower->compass.get_heading_deg());
}

unsigned short navigation::get_target() const
{
    return _target_angle;
}

void navigation::_set_target_angle(const unsigned short target_angle)
{
    _target_angle = target_angle % 360;
}

void navigation::_update_navigation()
{
    const navigation_pattern collision_pattern = _get_pattern_sensor();
    DEBUG_PRINTLN("NAVI SENSOR > " + String(get_current_pattern_str(collision_pattern)));
    // Critical mower stuck somewhere
    if (_delay_not_moving_cri.is_time_out())
    {
        mower->set_error(mower_status::ERROR_STUCK);
        return;
    }
    if (mower->gyro.is_moving() || mower->gps.get_gps_data()->speed > 1.f)
    {
        _delay_not_moving_cri.reset_delay();
        _delay_not_moving.reset_delay();
    }

    // Detect if mower is stuck when forward
    if ((_nav_patter == navigation_pattern::FIND_EXIT ||
         _nav_patter == navigation_pattern::KEEP_TARGET ||
         _nav_patter == navigation_pattern::FOLLOW_WIRE) &&
        _delay_not_moving.is_time_out(true) && !mower->gyro.is_moving())
    {
        DEBUG_PRINTLN("NAVI > Mower Stuck");
        _delay_full_reverse_pattern.reset_delay();
        _nav_patter = navigation_pattern::FULL_REVERSE;
        _delay_exit_pattern.reset_delay(3500); // time reverse
        _delay_not_moving.reset_delay();
        _process_pattern();
        return;
    }

    // IF MOWING
    if (mower->get_current_status() == mower_status::RUNNING)
    {
        if (_nav_patter == navigation_pattern::FULL_REVERSE && _delay_next_pattern.is_time_out(true, 3500) &&
            !mower->gyro.is_moving())
        {
            DEBUG_PRINTLN("NAVI > Mower Stuck go mowing");
            _start_mowing();
            return;
        }

        /*if (_nav_patter == navigation_pattern::TURN_TO_TARGET && _delay_next_pattern.is_time_out(true, 3500))
        {
            _start_mowing();
            return;
        }*/
    }
    ///////////////////////////
    if (_nav_patter != navigation_pattern::FIND_EXIT && collision_pattern == navigation_pattern::FIND_EXIT)
    {
        _nav_patter = navigation_pattern::FIND_EXIT;
        _delay_next_pattern.reset_delay();
        _delay_exit_pattern.reset_delay();
    }

    if (_nav_patter != navigation_pattern::FIND_EXIT && collision_pattern == navigation_pattern::FULL_REVERSE)
    {
        _nav_patter = navigation_pattern::FULL_REVERSE;
        _delay_next_pattern.reset_delay();
        _delay_exit_pattern.reset_delay();
        _delay_full_reverse_pattern.reset_delay();
    }

    // Exit found
    if (_nav_patter == navigation_pattern::FIND_EXIT && collision_pattern == navigation_pattern::KEEP_TARGET && _delay_exit_pattern.is_time_out())
    {
        update_target_angle();
        _nav_patter = navigation_pattern::KEEP_TARGET;
        _delay_next_pattern.reset_delay();
        _delay_exit_pattern.reset_delay();
    }

    // Not working for now
    if (_nav_patter == navigation_pattern::CIRCLE)
    {
        if (collision_pattern == navigation_pattern::KEEP_TARGET)
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
            _delay_next_pattern.reset_delay();
            _delay_exit_pattern.reset_delay();
        }
    }

    // FINAL CONDITION (outside perimeter)
    if (mower->get_current_status() != mower_status::RETURN_STATION)
    {
        if (_nav_patter != navigation_pattern::EXIT_WIRE_DETECT &&
            !mower->perim.is_inside())
        {
            _nav_patter = navigation_pattern::EXIT_WIRE_DETECT;
            _delay_next_pattern.reset_delay();
        }
        if (_nav_patter == navigation_pattern::EXIT_WIRE_DETECT && _delay_next_pattern.is_time_out(true, 5000))
        {
            _nav_patter = navigation_pattern::KEEP_TARGET;
            _delay_next_pattern.reset_delay();
        }
    }

    _process_pattern();
}

void navigation::_process_pattern()
{
    switch (_nav_patter)
    {
    case navigation_pattern::FIND_EXIT:
        _pattern_find_exit();
        break;

    case navigation_pattern::EXIT_WIRE_DETECT:
        _pattern_return_in_perim();
        break;

    case navigation_pattern::KEEP_TARGET:
        _pattern_keep_target();
        break;

    case navigation_pattern::TURN_TO_TARGET:
        _pattern_turn_target();
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

navigation::navigation_pattern navigation::_get_pattern_sensor()
{
    // Todo : optimize this
    const unsigned short speed = static_cast<unsigned short>(round(static_cast<float>(abs(mower->motor.get_speed_left()) + abs(mower->motor.get_speed_right())) / 2.4f));
    const unsigned short min_threshold = _nav_patter != navigation_pattern::FOLLOW_WIRE ? 75 : 65;
    const unsigned short max_threshold = _nav_patter != navigation_pattern::FOLLOW_WIRE ? 160 : 75;

    mower->sonar.get_collisions(_collision, constrain(speed, min_threshold, max_threshold));

    if (!mower->perim.is_inside() && _nav_patter != navigation_pattern::FOLLOW_WIRE)
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
    if (_delay_exit_pattern.is_time_out(true))
    {
        if (_collision[0] == 0 && _collision[1] == 0 && _collision[2] == 0)
        {
            _start_mowing();
            return;
        }
        _delay_full_reverse_pattern.reset_delay();
        _nav_patter = navigation_pattern::FULL_REVERSE;
        _pattern_full_reverse();
        return;
    }

    if ((_collision[0] >= MIN_COLLITION_DETECT || _collision[1] >= MIN_COLLITION_DETECT) && _collision[2] == 0)
    {
        mower->motor.set(SPEED_ROTATION, -SPEED_ROTATION);
        _delay_exit_pattern.reset_delay();
        return;
    }

    if (_collision[0] == 0 && _collision[1] == 0 && _collision[2] >= MIN_COLLITION_DETECT)
    {
        mower->motor.set(-SPEED_ROTATION, SPEED_ROTATION);
        _delay_exit_pattern.reset_delay();
        return;
    }

    float speed = abs(SPEED_REVERSE * (mower->gyro.get_AY() + 1.0f));
    mower->motor.set(-speed, -speed);
}

void navigation::_pattern_full_reverse()
{
    if (_collision[0] == 0 && _collision[1] == 0 && _collision[2] == 0 && _delay_exit_pattern.is_time_out())
    {
        _start_mowing(true, mower->compass.get_heading_deg() - 45);
        return;
    }
    if (_delay_full_reverse_pattern.is_time_out())
    {
        _start_mowing(true, mower->compass.get_heading_deg() - 45);
        return;
    }

    // Add factor mower angle position
    float speed = abs(SPEED_REVERSE * (mower->gyro.get_AY() + 1.0f));
    mower->motor.set(-speed, -speed);
}

void navigation::_pattern_return_in_perim()
{
    if (_delay_exit_pattern.is_time_out())
    {
        _start_mowing(true, mower->compass.get_heading_deg() - 45);
        return;
    }
    float speed = abs(SPEED_REVERSE * (mower->gyro.get_AY() + 1.0f));
    mower->motor.set(-speed, -speed);
}

void navigation::_pattern_keep_target(bool high_keeping, short max_speed)
{
    _drive_motor(0, max_speed);
    return;

    const unsigned short current_angle = mower->compass.get_heading_deg();
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

void navigation::_drive_motor(short correction, short max_speed)
{
    if (correction > 0)
    {
        if (!_delay_drive_motor.is_time_out())
        {
            mower->motor.set(max_speed, max_speed - correction);
        }
        return;
    }
    if (correction < 0)
    {
        if (!_delay_drive_motor.is_time_out())
        {
            mower->motor.set(max_speed - correction, max_speed);
        }
        return;
    }
    mower->motor.set(max_speed, max_speed);
}

void navigation::_pattern_turn_target()
{
    const unsigned short current_angle = mower->compass.get_heading_deg();
    short diff_angle = current_angle - _target_angle;
    if (abs(diff_angle) > 180)
    {
        diff_angle = 360 - abs(diff_angle);
    }

    if (diff_angle > TOLERANCE_TARGET)
    {
        mower->motor.set(SPEED_ROTATION, -SPEED_ROTATION);
        return;
    }
    if (diff_angle < -(TOLERANCE_TARGET))
    {
        mower->motor.set(-SPEED_ROTATION, SPEED_ROTATION);
        return;
    }
    _start_mowing();
}

void navigation::_pattern_follow_wire()
{
    if (!mower->perim.is_inside())
    {
        if (_delay_next_pattern.is_time_out())
        {
            if (_follow_wire_return_in)
            {
                // Keep rotating a little bit
                _delay_next_pattern.reset_delay(800);
                _follow_wire_return_in = false;
                return;
            }
            _follow_wire_return_in = false;
            _delay_next_pattern.reset_delay(2200);
            /*if (_delay_next_pattern.is_time_out())
            {
                _pattern_keep_target(true, 80);
                _delay_next_pattern.reset_delay(1600);
                return;
            }*/
            float speed = abs(SPEED_REVERSE * (mower->gyro.get_AY() + 1.0f));
            mower->motor.set(-speed, -speed);
            return;
        }
        return;
    }
    if (mower->perim.is_inside())
    {
        if (!_follow_wire_return_in)
        {
            _delay_drive_motor.reset_delay(2500);
            _drive_motor(180, 180);
            //_set_target_angle(_target_angle += 8); // new target
            _follow_wire_return_in = true;
            return;
        }
        if (_delay_drive_motor.is_time_out())
        {
            _drive_motor(0, 120);
            return;
        }
    }

    _drive_motor(0, 120);
}

void navigation::_pattern_leaving_station()
{
    if (!_delay_next_pattern.is_time_out())
    {
        mower->motor.set(-SPEED_REVERSE, -SPEED_REVERSE);
        return;
    }

    _start_mowing(true, mower->compass.get_heading_deg() - 45);
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
