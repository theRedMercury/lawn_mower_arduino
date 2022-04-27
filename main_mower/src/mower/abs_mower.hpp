/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#ifndef ABS_MOWER_H
#define ABS_MOWER_H

#include <Arduino.h>
#include <stdint.h>

class mower_manager;

class abs_mower_class
{
public:
    abs_mower_class(mower_manager &mower_m, String name = "NOT DEFINE");

    virtual inline void setup() = 0;
    virtual inline void update() = 0;

    abs_mower_class() = delete;
    abs_mower_class(const abs_mower_class &) = delete;
    abs_mower_class(abs_mower_class &&) = delete;
    abs_mower_class &operator=(const abs_mower_class &) = delete;
    abs_mower_class &operator=(abs_mower_class &&) = delete;

protected:
    String class_name;
    mower_manager *mower;
};

class delay_time_out
{
public:
    delay_time_out(const unsigned long ms_delay = 250);
    void reset_delay(const unsigned long ms_delay = 0);
    const bool is_time_out(const bool reset = false, const unsigned long add_delay = 0);

    delay_time_out(const delay_time_out &) = delete;
    delay_time_out(delay_time_out &&) = delete;
    delay_time_out &operator=(const delay_time_out &) = delete;
    delay_time_out &operator=(delay_time_out &&) = delete;

private:
    unsigned long _next_millis = 0;
    unsigned long _ms_delay;
};
#endif