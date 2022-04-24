/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#ifndef GYRO_H
#define GYRO_H

#include "../mower/abs_mower.hpp"
#include "../tools/tools.hpp"

#include "../libs/BMP280/BMP280.h"
#include "../libs/MPU9250/MPU9250.h"

#define COUNTER_MAX_TEMP_REFRESH 25
#define COUNTER_MOVING_REFRESH 40
#define COUNTER_IS_SAFE 10

class gyro_sensor : public abs_mower_class
{
public:
    using abs_mower_class::abs_mower_class;

    void setup();
    void update();

    const float get_temp() const;
    const float get_pression() const;

    const bool is_ready() const;
    const bool have_shock() const;
    const bool in_safe_status() const;
    const bool is_moving() const;

    // a XYZ
    const float get_ax() const;
    const float get_ay() const;
    const float get_az() const;

    // A XYZ
    const float get_AX() const;
    const float get_AY() const;
    const float get_AZ() const;

    const float get_ASqrt() const;

    // gyro
    const float get_GX() const;
    const float get_GY() const;
    const float get_GZ() const;

    const String get_json() const;

private:
    const bool _is_moving() const;
    const bool _is_safe() const;

    BMP280 _bmp280;
    MPU9250 _mpu9250;
    bool _gy91Ok = false;
    bool _have_shock = false;

    uint8_t _counter_moving = 0;
    uint8_t _counter_temp = 0;
    uint16_t _cumulation_is_safe = 0;

    float _aSqrt = 0.f;
    double _current_temp, _current_pression = 0; // degC, mBar
    XYZ_FLOAT _accel;
    XYZ_FLOAT _Accel;
    XYZ_FLOAT _gyro;
};
#endif