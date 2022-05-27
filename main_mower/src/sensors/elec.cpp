/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "elec.hpp"
#include "../mower/mower.hpp"
#include "../tools/adc_manager.hpp"

void elec_sensor::setup()
{
    DEBUG_PRINT("SETUP : ");
    DEBUG_PRINT(class_name);
    pinMode(PIN_A_AMP, INPUT);
    pinMode(PIN_A_VOLT, INPUT);
    DEBUG_PRINTLN(" : DONE");
}
void elec_sensor::update()
{
    update_volt();
    update_amp();

    // is_charging
    if (_current_amp > AMP_CHARGING)
    {
        _cumulation_charge_max_time = BARERRY_TIME_OUT;
    }
    else
    {
        if (_cumulation_charge_max_time == 0)
        {
            _cumulation_charge_max_time++;
        }
        _cumulation_charge_max_time--;
    }

    // is_battery_charged
    if (_current_volt >= BATERRY_MAX)
    {
        _cumulation_volt_max_time++;
        if (_cumulation_volt_max_time > BARERRY_TIME_OUT + 1)
        {
            _cumulation_volt_max_time--;
        }
    }
    else
    {
        _cumulation_volt_max_time = 0;
    }

    // is_battery_low
    if (_current_volt < BATERRY_MIN)
    {
        _cumulation_volt_min_time++;
        if (_cumulation_volt_min_time > BARERRY_TIME_OUT)
        {
            _cumulation_volt_min_time--;
        }
    }
    else
    {
        _cumulation_volt_min_time = 0;
    }

    // Battery process
    if (is_charging() && mower->get_current_status() != mower_status::CHARGING)
    {
        mower->set_current_status(mower_status::CHARGING);
        return;
    }
    if (!is_charging() && mower->get_current_status() == mower_status::CHARGING)
    {
        mower->set_current_status(mower_status::READY);
        return;
    }
    if (is_battery_critical() && mower->get_current_status() != mower_status::ERROR_POWER)
    {
        mower->set_error(mower_status::ERROR_POWER);
        return;
    }
    if (!is_battery_critical() && mower->get_current_status() == mower_status::ERROR_POWER)
    {
        mower->set_current_status(mower_status::READY);
        return;
    }
}

void elec_sensor::update_volt()
{
    // float R1 = 30000.0f;
    // float R2 = 7500.0f;

    // 318 = 8.6
    // 415 = 10.91
    // 460 = 12
    // 505 = 13

    const unsigned short val = adc_manager::analogue_read_channel(PIN_A_VOLT, 16);
    _current_volt = constrain(static_cast<float>(val) / 38.3f, 0, 24); //(R2 / (R1 + R2));

    DEBUG_PRINT("Volt brut >\t");
    DEBUG_PRINT(val);
    DEBUG_PRINT("  >\t");
    DEBUG_PRINT("Volt  >\t");
    DEBUG_PRINTLN(_current_volt);
}

void elec_sensor::update_amp()
{
    // 510 = 0.00  - 0
    // 516 = 0.32  - 7
    // 524 = 0.76  - 14
    const unsigned short val = adc_manager::analogue_read_channel(PIN_A_AMP, 16);
    _current_amp = constrain((static_cast<float>(val) - 510), 0, 512) * 0.05f;

    DEBUG_PRINT("Amp brut  >\t");
    DEBUG_PRINT(val);
    DEBUG_PRINT("  >\t");
    DEBUG_PRINT("Amps >\t");
    DEBUG_PRINTLN(_current_amp);
}

const bool elec_sensor::is_battery_charged() const
{
    return (_cumulation_volt_max_time >= BARERRY_TIME_OUT);
}

const bool elec_sensor::is_battery_low() const
{
    return (_cumulation_volt_min_time >= BARERRY_TIME_OUT);
}

const bool elec_sensor::is_battery_critical() const
{
    return (_current_volt <= BATERRY_CRI);
}

const bool elec_sensor::is_charging() const
{
    return (_cumulation_charge_max_time >= BARERRY_TIME_OUT - 120);
}

const float elec_sensor::get_curent_volt() const
{
    return _current_volt;
}

const float elec_sensor::get_curent_amp() const
{
    return _current_amp;
}

const String elec_sensor::get_json() const
{
    return "{\"volt\":\"" + String(_current_volt) + "\",\"amp\":\"" +
           String(_current_amp) + "\",\"char\":\"" + String(is_charging()) + "\"}";
}
