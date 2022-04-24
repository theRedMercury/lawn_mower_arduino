/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */
#include "mower.hpp"

void mower_manager::setup()
{
    // SETUP MOWER
    _current_status = mower_status::WAITING;

    motor_blade.setup();
    motor.setup();
    elec.setup();
    gyro.setup();
    gps.setup();
    rain.setup();
    sonar.setup();
    lcd.setup();
    perim.setup();
    nav.setup();
    wifi.setup();
    schedul.setup();

    lcd.clear();
    lcd.on_backlight();

    _current_status = mower_status::READY;
    if (!gyro.is_ready() || !gps.is_ready())
    {
        _current_status = mower_status::ERROR_INIT;
    }
    DEBUG_PRINTLN("SETUP : FINISH \n");
}

void mower_manager::update()
{
    if (_current_status == mower_status::ERROR_INIT && gyro.is_ready() && gps.is_ready())
    {
        _current_status = mower_status::READY;
    }

    // UPDATE SENSOR ================================
    gps.update();
    rain.update();
    sonar.update();
    gyro.update();
    elec.update();
    perim.update();

    // NAV ===========================================
    nav.update();

    // MOTOR =========================================
    motor.update();
    motor_blade.update();

    // SCREEN ========================================
    lcd.update();

    // WIFI ==========================================
    wifi.update();
}

void mower_manager::set_error(const mower_status error)
{
    motor.stop();
    motor_blade.stop();
    set_current_status(error);
}

const char *mower_manager::get_current_status_str() const
{
    switch (_current_status)
    {
    case mower_status::READY:
        return "READY   ";
    case mower_status::RUNNING:
        return "RUNNING ";
    case mower_status::WAITING:
        return "WAITING ";
    case mower_status::LEAVING_STATION:
        return "LEAV STA";
    case mower_status::RETURN_STATION:
        return "RETURN_S";
    case mower_status::CHARGING:
        return "CHARGING";
    case mower_status::TESTING_NAV:
    case mower_status::TESTING_NAV_02:
        return "TESTING ";
    case mower_status::ERROR_INIT:
        return "ERR INIT";
    case mower_status::ERROR_LOST_WIRE:
        return "ERLOST W";
    case mower_status::ERROR:
        return "ERROR   ";
    default:
        return "UNKNOW";
    }
}

const mower_status mower_manager::get_current_status() const
{
    return _current_status;
}

void mower_manager::set_current_status(const mower_status new_status)
{
    if (_current_status == mower_status::ERROR_INIT)
    {
        return;
    }
    // Start mowing
    if (new_status == mower_status::RUNNING && _current_status != mower_status::RUNNING)
    {
        nav.start_mowing();
    }

    _current_status = new_status;
    wifi.send_msg("status", get_current_status_str());
}