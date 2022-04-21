/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#ifndef NAVIGATION_H
#define NAVIGATION_H
#include "../mower/abs_mower.hpp"

#define MAX_DELAY_COUNTER 14 // const unsigned short _max_delay_counter = 14;
#define MAX_CIRCLE_DELAY_COUNTER 5000

class navigation : public abs_mower_class
{
private:
    enum class navigation_pattern
    {
        BYPASS,
        STOP,
        KEEP_TARGET,
        FIND_EXIT,
        EXIT_WIRE_DETECT,
        FULL_REVERSE,
        CIRCLE,
        FOLLOW_WIRE,
        LEAVING_STATION,
        ENTRING_STATION
    };

    enum class navigation_exit
    {
        BYPASS,
        FULL_REVERSE,
        TURN_LEFT,
        TURN_RIGHT
    };

public:
    using abs_mower_class::abs_mower_class;
    void setup();
    void update();

    void update_target_angle();
    const char *get_current_pattern_str(const navigation_pattern n) const;

private:
    float _target_angle = 0.f;
    bool _collision[3] = {true, true, true};
    bool _wire_find = false;
    navigation_pattern _nav_patter = navigation_pattern::BYPASS;
    navigation_exit _nav_exit = navigation_exit::BYPASS;

    unsigned short _delay_next_pattern = 0;
    unsigned short _delay_exit_pattern = 0;
    unsigned long _circle_milli = 0;

    void _update();
    void _process_pattern();

    const navigation_pattern _get_pattern_sensor();

    void _pattern_find_exit();
    void _pattern_full_reverse();
    void _pattern_return_in_perim();
    void _pattern_keep_target(uint8_t max_speed = 255);
    void _pattern_follow_wire();
    void _pattern_leaving_station();
    void _pattern_entering_station();
    void _charching();
};
#endif