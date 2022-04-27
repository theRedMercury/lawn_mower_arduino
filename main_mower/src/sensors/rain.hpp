/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#ifndef RAIN_H
#define RAIN_H

#include "../mower/abs_mower.hpp"

#define PIN_A_RAIN PIN_A3  // A3
#define THRESHOLD_RAIN 512 // MAX 1024

class rain_sensor : public abs_mower_class
{
public:
    using abs_mower_class::abs_mower_class;

    void setup();
    void update();

    const unsigned short get_value() const;
    const bool is_rainning() const;

private:
    unsigned short _current_rain_value;
};
#endif