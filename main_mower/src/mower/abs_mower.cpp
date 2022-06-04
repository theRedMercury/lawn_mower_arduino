/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "abs_mower.hpp"

// abs_mower_class
abs_mower_class::abs_mower_class(mower_manager &mower_m, String name)
{
    mower = &mower_m;
    class_name = name;
}

// delay_time_out
delay_time_out::delay_time_out(const unsigned long ms_delay)
{
    _ms_delay = ms_delay;
}

void delay_time_out::reset_delay(const unsigned long ms_delay_overwrite)
{
    if (ms_delay_overwrite != 0)
    {
        _next_millis = millis() + ms_delay_overwrite;
    }
    else
    {
        _next_millis = millis() + _ms_delay;
    }
}

bool delay_time_out::is_time_out(const bool reset, const unsigned long add_delay)
{
    const bool is_timeout = (_next_millis + add_delay) <= millis();
    if (is_timeout && reset)
    {
        reset_delay();
    }
    return is_timeout;
}