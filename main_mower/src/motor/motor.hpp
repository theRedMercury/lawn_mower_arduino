/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#ifndef MOTOR_H
#define MOTOR_H

#include "../mower/abs_mower.hpp"
#include "../tools/tools.hpp"

#define PIN_MOTOR_LEFT_FORWARD 6
#define PIN_MOTOR_LEFT_BACKWARD 7
#define PIN_MOTOR_LEFT_SPEED 5

#define PIN_MOTOR_RIGHT_FORWARD 4
#define PIN_MOTOR_RIGHT_BACKWARD 3
#define PIN_MOTOR_RIGHT_SPEED 2

class motor_control : public abs_mower_class
{
public:
    using abs_mower_class::abs_mower_class;
    void setup();
    void update();
    void stop();

    void set(short left_speed = 0, short right_speed = 0);

    bool is_running() const;
    short get_speed_left() const;
    short get_speed_right() const;

private:
    struct PACKING motor_stuct
    {
        float computed_speed = 0.f;
        short target_speed = 0;
        unsigned char real_speed = 0;
        unsigned long last_set_speed_time = 0;
        bool is_stop = false;
        unsigned char PIN_FORWARD;
        unsigned char PIN_BACKWARD;
        unsigned char PIN_SPEED;
    };

    void _stop_motor(motor_stuct *motor);
    void _update_motor_pin(motor_stuct *motor);
    void _update_motor_speed(motor_stuct *motor);

    motor_stuct _motor_left;
    motor_stuct _motor_right;
    unsigned int _security_count = 0;
};
#endif