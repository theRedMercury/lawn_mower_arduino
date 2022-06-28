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

#define MAX_TEMP_WARNING 45.5f
#define MAX_TEMP_CRITICAL 55.f

class gyro_sensor : public abs_mower_class
{
public:
    using abs_mower_class::abs_mower_class;

    void setup();
    void update();

    float get_temp() const;
    float get_pression() const;

    bool is_ready() const;
    bool have_shock() const;
    bool is_taken() const;
    bool in_safe_status() const;
    bool is_moving() const;

    bool is_temp_warning() const;
    bool is_temp_critical() const;

    // a XYZ
    float get_ax() const;
    float get_ay() const;
    float get_az() const;

    // A XYZ
    float get_AX() const;
    float get_AY() const;
    float get_AZ() const;

    float get_ASqrt() const;

    // gyro
    float get_GX() const;
    float get_GY() const;
    float get_GZ() const;

    String get_json() const;

    void start_init_gyro();
    void stop_init_gyro();
    void reset_init_gyro();

private:
    bool _is_moving() const;
    bool _is_safe() const;

    BMP280 _bmp280;
    MPU9250 _mpu9250; // it's a mpu6050
    // MPU6050 _mpu6050;
    bool _gy91Ok = false;
    bool _have_shock = false;
    bool _is_taken = false;

    short _counter_moving = 0;
    unsigned char _counter_temp = 0;
    short _cumulation_is_safe = 0;
    short _cumulation_is_taken = 0;

    float _aSqrt = 0.f;
    double _current_temp, _current_pression = 0; // degC, mBar
    XYZ_FLOAT _accel;
    XYZ_FLOAT _Accel; // Pitch Roll Yaw in degree
    XYZ_FLOAT _gyro;
    XYZ_FLOAT _gyro_min;
    XYZ_FLOAT _gyro_max;
    bool _set_gyro_min_max = false;
};
#endif