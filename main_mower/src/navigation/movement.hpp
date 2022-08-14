/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#ifndef MOVEMENT_H
#define MOVEMENT_H
#include "../mower/abs_mower.hpp"
#include "../tools/tools.hpp"

#define DEFAULT_DELAY_MOVEMENT_MS 1800
#define SIZE_LIST_MOVEMENT 3

class movement : public abs_mower_class
{
public:
    using abs_mower_class::abs_mower_class;

    enum class movement_process : char
    {
        WAIT,
        FORWARD,
        TURN_LEFT,
        TURN_RIGHT,
        FULL_REVERSE,
        WITH_CORRECTION,
    };

    struct PACKING movement_action
    {
        movement_process movement = movement_process::WAIT;
        short delay = 0;
        unsigned short max_speed = DEFAULT_DELAY_MOVEMENT_MS;
    };

    void setup();
    void update();

    const char get_current_pattern_str(const movement_process n) const;
    movement_process get_current_movement() const { return _current_movement; };

    bool not_task() const;
    bool is_waiting() const;

    void forward(bool stack_list = false, short max_delay_ms = -1, unsigned short max_speed = 230);
    void turn_left(bool stack_list = false, short max_delay_ms = DEFAULT_DELAY_MOVEMENT_MS, unsigned short max_speed = 230);
    void turn_right(bool stack_list = false, short max_delay_ms = DEFAULT_DELAY_MOVEMENT_MS, unsigned short max_speed = 230);
    void reverse(bool stack_list = false, short max_delay_ms = DEFAULT_DELAY_MOVEMENT_MS, unsigned short max_speed = 230);

    void with_correction(short correction = 0, short max_delay_ms = DEFAULT_DELAY_MOVEMENT_MS, unsigned short max_speed = 230);

private:
    void _apply_current_mouvement();
    void _push_movement(movement_process mov, short delay, unsigned short max_speed);

    movement_action _list_movement[SIZE_LIST_MOVEMENT];
    unsigned char _list_index = 0;
    unsigned char _list_inde_proced = 0;
    bool _list_empty = true;

    unsigned short _max_speed = 0;
    short _correction = 0;
    movement_process _current_movement = movement_process::WAIT;
    short _current_delay = -1;
    movement_process _next_movement = movement_process::WAIT;
    short _next_delay = -1;
    delay_time_out _delay_current_movement{0};
};

#endif