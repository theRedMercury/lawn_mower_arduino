/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "gyro.hpp"
#include "../mower/mower.hpp"

#define COUNTER_MAX_TEMP_REFRESH 25
#define COUNTER_MOVING_REFRESH 20
#define COUNTER_IS_SAFE 10

void gyro_sensor::setup()
{
    DEBUG_PRINT("SETUP : ");
    DEBUG_PRINT(class_name);
    _have_shock = false;

    // GY-91 is ok
    _gy91Ok = _bmp280.begin();
    _bmp280.setOversampling(4);
    _mpu9250.beginAccel();
    _mpu9250.beginGyro();
    _gy91Ok |= _bmp280.getError();
    _counter_temp = COUNTER_MAX_TEMP_REFRESH;
    _counter_moving = 0;
    _cumulation_is_safe = COUNTER_IS_SAFE;
    DEBUG_PRINTLN(" : DONE");
}

void gyro_sensor::update()
{
    _mpu9250.accelUpdate();
    _mpu9250.gyroUpdate();

    char result = _bmp280.startMeasurment();
    if (result != 0 && _counter_temp == COUNTER_MAX_TEMP_REFRESH)
    {
        delay(result);
        _bmp280.getTemperatureAndPressure(_current_temp, _current_pression);
        _counter_temp = 0;
    }
    _counter_temp++;

    _accel.x = _mpu9250.accelX();
    _accel.y = _mpu9250.accelY();
    _accel.z = _mpu9250.accelZ();
    _aSqrt = _mpu9250.accelSqrt();

    if (_aSqrt > 2.5f) // Force 5G
    {
        _have_shock = true;
    }

    _gyro.x = _mpu9250.gyroX();
    _gyro.y = _mpu9250.gyroY();
    _gyro.z = _mpu9250.gyroZ();

    // Convert radians to degrees
    _Accel.x = atan(_accel.x / (sqrt(_accel.y * _accel.y + _accel.z * _accel.z))) * 180.f / M_PI;
    _Accel.y = atan(_accel.y / (sqrt(_accel.x * _accel.x + _accel.z * _accel.z))) * 180.f / M_PI;
    _Accel.z = atan((sqrt(_accel.x * _accel.x + _accel.y * _accel.y)) / _accel.z) * 180.f / M_PI;

    // is_moving
    if (_is_moving())
    {
        _counter_moving = COUNTER_MOVING_REFRESH;
    }
    else
    {
        if (_counter_moving == 0)
        {
            _counter_moving++;
        }
        _counter_moving--;
    }

    // is_safe
    if (_is_safe())
    {
        _cumulation_is_safe++;
        if (_cumulation_is_safe > COUNTER_IS_SAFE)
        {
            _cumulation_is_safe--;
        }
    }
    else
    {
        _cumulation_is_safe--;
        if (_cumulation_is_safe == 0)
        {
            _cumulation_is_safe = 0;
        }
    }

    // Print degrees
    DEBUG_PRINT("ACCEL > ");
    DEBUG_PRINT("AX = ");
    DEBUG_PRINT(_Accel.x);
    DEBUG_PRINT(" AY = ");
    DEBUG_PRINT(_Accel.y);
    DEBUG_PRINT(" AZ = ");
    DEBUG_PRINTLN(_Accel.y);

    DEBUG_PRINT("WEATHE> ");
    DEBUG_PRINT(_current_temp, 2);
    DEBUG_PRINT(" (degC)\t");
    DEBUG_PRINT("P : ");
    DEBUG_PRINT(_current_pression, 2);
    DEBUG_PRINTLN(" (mBar)");
    DEBUG_PRINT("GYRO >");
    DEBUG_PRINT("\taccelX: " + String(_accel.x));
    DEBUG_PRINT("\taccelY: " + String(_accel.y));
    DEBUG_PRINT("\taccelZ: " + String(_accel.z));
    DEBUG_PRINT("\taccelSqrt: " + String(_aSqrt));
    DEBUG_PRINT("\tgyroX: " + String(_gyro.x));
    DEBUG_PRINT("\tgyroY: " + String(_gyro.y));
    DEBUG_PRINT("\tgyroZ: " + String(_gyro.z));
    DEBUG_PRINTLN("");
}

const float gyro_sensor::get_temp() const
{
    return _current_temp;
}

const float gyro_sensor::get_pression() const
{
    return _current_pression;
}

const bool gyro_sensor::is_ready() const
{
    return _gy91Ok;
}

const bool gyro_sensor::have_shock() const
{
    return _have_shock;
}

const bool gyro_sensor::in_safe_status() const
{
    return _cumulation_is_safe > COUNTER_IS_SAFE - 2;
}

const bool gyro_sensor::is_moving() const
{
    return _counter_moving > 0;
}

// a XYZ
const float gyro_sensor::get_ax() const
{
    return _accel.x;
}

const float gyro_sensor::get_ay() const
{
    return _accel.y;
}

const float gyro_sensor::get_az() const
{
    return _accel.z;
}

// A XYZ
const float gyro_sensor::get_AX() const
{
    return _Accel.x;
}
const float gyro_sensor::get_AY() const
{
    return _Accel.y;
}
const float gyro_sensor::get_AZ() const
{
    return _Accel.z;
}

const float gyro_sensor::get_ASqrt() const
{
    return _aSqrt;
}

// gyro
const float gyro_sensor::get_GX() const
{
    return _gyro.x;
}

const float gyro_sensor::get_GY() const
{
    return _gyro.y;
}

const float gyro_sensor::get_GZ() const
{
    return _gyro.z;
}

const String gyro_sensor::get_json() const
{
    return "{\"temp\":\"" + String(_current_temp) + "\",\"psi\":\"" +
           String(_current_pression) + "\"}";
}

const bool gyro_sensor::_is_moving() const
{
    const float threshold_g = 10.f;
    return abs(_gyro.x) > threshold_g || abs(_gyro.y) > threshold_g || abs(_gyro.z) > threshold_g;
}

const bool gyro_sensor::_is_safe() const
{
    const float threshold_a = 15.0f;
    return abs(_Accel.x) < threshold_a && abs(_Accel.y) < threshold_a && abs(_Accel.z) < threshold_a;
}
