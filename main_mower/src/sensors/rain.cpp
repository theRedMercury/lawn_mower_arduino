/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "rain.hpp"
#include "../mower/mower.hpp"
#include "../tools/adc_manager.hpp"

void rain_sensor::setup()
{
    DEBUG_PRINT("SETUP : ");
    DEBUG_PRINT(class_name);
    pinMode(PIN_A_RAIN, INPUT);
    DEBUG_PRINTLN(" : DONE");
}
void rain_sensor::update()
{
    _current_rain_value = adc_manager::analogue_read_channel(PIN_A_RAIN);
    DEBUG_PRINTLN("RAIN >\t" + String(_current_rain_value));
}
const uint16_t rain_sensor::get_value() const
{
    return _current_rain_value;
}
const bool rain_sensor::is_rainning() const
{
    return _current_rain_value > THRESHOLD_RAIN;
}