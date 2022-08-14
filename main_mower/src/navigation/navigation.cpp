/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "navigation.hpp"
#include "../mower/mower.hpp"

#define SPEED_MAX 255
#define SPEED_MINIMAL 160
#define SPEED_REVERSE 122
#define SPEED_ROTATION 130
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
navigation::navigation_pattern navigation::get_nav_pattern() const
{
    return _nav_patter;
}

void navigation::start_move()
{
    update_target_angle();
    _delay_not_moving.reset_delay();
    _delay_not_moving_cri.reset_delay();
    _delay_next_pattern.reset_delay();
    if (mower->get_current_status() == mower_status::RETURN_STATION)
    {
        _nav_patter = navigation_pattern::FOLLOW_WIRE;
        return;
    }
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
    DEBUG_PRINTLN("_start_mowing");
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
    // TODO : Uncomment
    /*  if (_delay_not_moving_cri.is_time_out())
      {
          mower->set_error(mower_status::ERROR_STUCK);
          return;
      }
      if (mower->gyro.is_moving() || mower->gps.get_gps_data()->speed > 1.f)
      {
          _delay_not_moving_cri.reset_delay();
          _delay_not_moving.reset_delay();
      }*/

    // Detect if mower is stuck when forward
    // TODO : Uncomment
    /*if ((_nav_patter == navigation_pattern::FIND_EXIT ||
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
    }*/

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
        start_mowing();
        /*update_target_angle();
        _nav_patter = navigation_pattern::KEEP_TARGET;
        _delay_next_pattern.reset_delay();
        _delay_exit_pattern.reset_delay();*/
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
            _wire_exit_detected = true;
            _nav_patter = navigation_pattern::EXIT_WIRE_DETECT;
            _delay_next_pattern.reset_delay();
            _delay_exit_wire.reset_delay();
        }
        // We estiamte after 5 sec, we are back in perimeter
        if (_nav_patter == navigation_pattern::EXIT_WIRE_DETECT && _delay_next_pattern.is_time_out(true, 5000))
        {
            start_mowing();
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

    if (_collision[0] >= MIN_COLLITION_DETECT || _collision[1] >= MIN_COLLITION_DETECT || _collision[2] >= MIN_COLLITION_DETECT)
    {
        return navigation_pattern::FIND_EXIT;
    }
    return navigation_pattern::KEEP_TARGET;
}

void navigation::_pattern_find_exit()
{
    _delay_exit_pattern.reset_delay();
    if (_collision[0] == 0 && _collision[1] == 0 && _collision[2] == 0)
    {
        _start_mowing();
        return;
    }

    if (_collision[0] >= MIN_COLLITION_DETECT && _collision[1] >= MIN_COLLITION_DETECT && _collision[2] >= MIN_COLLITION_DETECT)
    {
        mower->mov.reverse();
        return;
    }

    float speed = abs(SPEED_REVERSE * (abs(mower->gyro.get_AY()) + 1.0f));

    if (_collision[1] >= MIN_COLLITION_DETECT)
    {

        if (_collision[0] >= MIN_COLLITION_DETECT && _collision[2] == 0)
        {
            mower->mov.turn_right();
            return;
        }
        if (_collision[2] >= MIN_COLLITION_DETECT && _collision[0] == 0)
        {
            mower->mov.turn_left();
            _delay_exit_pattern.reset_delay();
            return;
        }
        mower->mov.turn_right();
        return;
    }

    if (_collision[0] >= MIN_COLLITION_DETECT && _collision[2] == 0)
    {
        mower->mov.turn_right();
        _delay_exit_pattern.reset_delay();
        return;
    }

    if (_collision[0] == 0 && _collision[2] >= MIN_COLLITION_DETECT)
    {
        mower->mov.turn_left();
        _delay_exit_pattern.reset_delay();
        return;
    }
    mower->mov.reverse();
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

    mower->mov.reverse();
    // Add factor mower angle position
    // float speed = abs(SPEED_REVERSE * (mower->gyro.get_AY() + 1.0f));
    // mower->motor.set(-speed, -speed);
}

void navigation::_pattern_return_in_perim()
{
    if (_delay_exit_wire.is_time_out())
    {
        if (_wire_exit_detected)
        {
            mower->mov.turn_right();
            _wire_exit_detected = false;
            return;
        }
        if (!_wire_exit_detected && _delay_exit_pattern.is_time_out())
        {
            _start_mowing(true, mower->compass.get_heading_deg() - 45);
            return;
        }
        return;
    }
    mower->mov.reverse();
    // float speed = abs(SPEED_REVERSE * (mower->gyro.get_AY() + 1.0f));
    // mower->motor.set(-speed, -speed);
}

void navigation::_pattern_keep_target(bool high_keeping, short max_speed)
{
    mower->mov.forward(false);
}

void navigation::_pattern_follow_wire()
{
    if (mower->perim.get_simulate_is_inside())
    {

        if (mower->mov.not_task())
        {
            mower->mov.forward(true, 1000, SPEED_MINIMAL);
            mower->mov.turn_right(true, 1800, SPEED_MINIMAL);
        }
        // mower->set_error(mower_status::WAITING);
        return;
    }

    if (!_wire_find)
    {

        mower->mov.forward(-1, SPEED_MINIMAL);

        if (!mower->perim.is_inside())
        {
            if (mower->mov.not_task())
            {
                mower->mov.reverse(true);
                mower->mov.turn_right(true);
            }
            _wire_find = true;
            return;
            /*_wire_find = true;
            _delay_full_reverse_pattern.reset_delay();
            float speed = abs(SPEED_REVERSE * (mower->gyro.get_AY() + 1.0f));
            mower->motor.set(-speed, -speed);*/
        }
        return;
    }

    // WIRE FOUND --------------------------------
    if (!mower->mov.is_waiting())
    {
        return;
    }

    if (!mower->perim.is_inside())
    {
        if (!_out_wire)
        {
            _correction_follow_wire = 0;
            _out_wire = true;
            _delay_exit_wire.reset_delay();
        }

        if (_delay_exit_wire.is_time_out())
        {
            mower->mov.reverse();
        }
        _correction_follow_wire += 5;
    }

    if (mower->perim.is_inside())
    {
        if (_out_wire)
        {
            _correction_follow_wire = 0;
            _out_wire = false;
        }
        _delay_exit_wire.reset_delay();
        _correction_follow_wire -= 5;
    }

    _correction_follow_wire = constrain(_correction_follow_wire, -180, 180);
    mower->mov.with_correction(_correction_follow_wire, 800, 180);
}

void navigation::_pattern_leaving_station()
{
    if (mower->mov.not_task())
    {
        mower->mov.reverse(true);
        mower->mov.turn_right(true);
    }
    if (mower->mov.is_waiting())
    {
        _start_mowing(true, mower->compass.get_heading_deg() - 45);
    }

    /*if (!_delay_next_pattern.is_time_out())
    {
        mower->motor.set(-SPEED_REVERSE, -SPEED_REVERSE);
        return;
    }*/
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
