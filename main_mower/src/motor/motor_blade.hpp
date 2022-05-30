/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#ifndef MOTOR_BLADE_H
#define MOTOR_BLADE_H

#include "../mower/abs_mower.hpp"

#define PIN_RPWM 13
#define PIN_L_EN 12
#define PIN_R_EN 11
#define PIN_RELAY 26
#define MAX_RPWW_SPEED 220

class motor_control_blade : public abs_mower_class
{
public:
    using abs_mower_class::abs_mower_class;
    void setup();
    void update();

    void stop();
    void on();
    bool is_on() const;

private:
    bool _blade_on = true; // force to stop at setup
    unsigned char _speed = 0;
};
#endif