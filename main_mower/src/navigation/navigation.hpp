/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#ifndef NAVIGATION_H
#define NAVIGATION_H
#include "../mower/abs_mower.hpp"

#define MAX_DELAY_PATTERN_MS 250
#define MAX_DELAY_PATTERN_EXIT_MS 500
#define MAX_DELAY_NOT_MOUVING_MS 20 * 1000 // 20 sec
#define MAX_CIRCLE_DELAY_COUNTER 5000
#define MIN_COLLITION_DETECT 3

class navigation : public abs_mower_class
{
public:
    enum class navigation_pattern : char
    {
        BYPASS,
        STOP,
        KEEP_TARGET,
        TURN_TO_TARGET,
        FIND_EXIT,
        EXIT_WIRE_DETECT,
        FULL_REVERSE,
        CIRCLE,
        FOLLOW_WIRE,
        LEAVING_STATION,
        ENTRING_STATION
    };

    enum class navigation_exit : char
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

    void start_mowing();

    void update_target_angle();
    const char *get_current_pattern_str(const navigation_pattern n) const;
    navigation_pattern get_nav_pattern();
    unsigned short get_target() const;

    short correction;

private:
    unsigned short _target_angle = 0;
    unsigned char _collision[3] = {0, 0, 0};
    bool _wire_find = false;
    navigation_pattern _nav_patter = navigation_pattern::BYPASS;
    navigation_exit _nav_exit = navigation_exit::BYPASS;

    unsigned long _circle_milli = 0;

    void _start_mowing(const bool wait_target = false, const unsigned short target = 1000);

    void _update_navigation();
    void _process_pattern();

    void _set_target_angle(const unsigned short target_angle);

    navigation_pattern _get_pattern_sensor();

    void _pattern_find_exit();
    void _pattern_full_reverse();
    void _pattern_return_in_perim();
    void _pattern_keep_target(bool high_keeping = false, unsigned char max_speed = 255);
    void _pattern_turn_target();
    void _pattern_follow_wire();
    void _pattern_leaving_station();
    void _pattern_entering_station();
    void _charching();

    delay_time_out _delay_next_pattern{MAX_DELAY_PATTERN_MS};
    delay_time_out _delay_exit_pattern{MAX_DELAY_PATTERN_EXIT_MS};
    delay_time_out _delay_not_moving{MAX_DELAY_NOT_MOUVING_MS};
};
#endif