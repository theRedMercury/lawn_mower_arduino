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

#endif