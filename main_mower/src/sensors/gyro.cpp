/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "gyro.hpp"
#include "../mower/mower.hpp"

#define COUNTER_MAX_TEMP_REFRESH 25
#define COUNTER_MOVING_REFRESH 40
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
    char result = _bmp280.startMeasurment();
    if (result != 0 && _counter_temp >= COUNTER_MAX_TEMP_REFRESH)
    {
        delay(result);
        _bmp280.getTemperatureAndPressure(_current_temp, _current_pression);
        _counter_temp = 0;
    }
    _counter_temp++;

    _mpu9250.accelUpdate();
    _mpu9250.gyroUpdate();

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

    // Save min / max during motor blade on
    if (_set_gyro_min_max)
    {
        _gyro_max.x = _gyro.x > _gyro_max.x ? _gyro.x : _gyro_max.x;
        _gyro_min.x = _gyro.x < _gyro_min.x ? _gyro.x : _gyro_min.x;
        _gyro_max.y = _gyro.y > _gyro_max.y ? _gyro.y : _gyro_max.y;
        _gyro_min.y = _gyro.y < _gyro_min.y ? _gyro.y : _gyro_min.y;
        _gyro_max.z = _gyro.z > _gyro_max.z ? _gyro.z : _gyro_max.z;
        _gyro_min.z = _gyro.z < _gyro_min.z ? _gyro.z : _gyro_min.z;
    }

    // Convert Pitch Roll Yaw radians
    _Accel.x = atan(_accel.x / (sqrt(_accel.y * _accel.y + _accel.z * _accel.z))); // * 180.f / M_PI;
    _Accel.y = atan(_accel.y / (sqrt(_accel.x * _accel.x + _accel.z * _accel.z))); // * 180.f / M_PI;
    _Accel.z = atan((sqrt(_accel.x * _accel.x + _accel.y * _accel.y)) / _accel.z); // * 180.f / M_PI;

    // is_moving
    if (_is_moving())
    {
        _counter_moving = COUNTER_MOVING_REFRESH;
    }
    else
    {
        if (_counter_moving != 0)
        {
            _counter_moving--;
        }
    }
    _counter_moving = constrain(_counter_moving, 0, COUNTER_MOVING_REFRESH);

    // is_safe
    if (_is_safe())
    {
        _cumulation_is_safe++;
    }
    else
    {
        if (_cumulation_is_safe != 0)
        {
            _cumulation_is_safe--;
        }
    }
    _cumulation_is_safe = constrain(_cumulation_is_safe, 0, COUNTER_IS_SAFE);

    // Print degrees
    DEBUG_PRINT("SAFE > ");
    DEBUG_PRINTLN(_cumulation_is_safe);
    DEBUG_PRINT("MOVING > ");
    DEBUG_PRINTLN(_counter_moving);
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

float gyro_sensor::get_temp() const
{
    return _current_temp;
}

float gyro_sensor::get_pression() const
{
    return _current_pression;
}

bool gyro_sensor::is_ready() const
{
    return _gy91Ok;
}

bool gyro_sensor::have_shock() const
{
    return _have_shock;
}

bool gyro_sensor::in_safe_status() const
{
    return _cumulation_is_safe > COUNTER_IS_SAFE - 5;
}

bool gyro_sensor::is_moving() const
{
    return _counter_moving > 0;
}

bool gyro_sensor::is_temp_warning() const
{
    return _current_temp >= MAX_TEMP_WARNING;
}
bool gyro_sensor::is_temp_critical() const
{
    return _current_temp >= MAX_TEMP_CRITICAL;
}

// a XYZ
float gyro_sensor::get_ax() const
{
    return _accel.x;
}

float gyro_sensor::get_ay() const
{
    return _accel.y;
}

float gyro_sensor::get_az() const
{
    return _accel.z;
}

// A XYZ
float gyro_sensor::get_AX() const
{
    return _Accel.x;
}
float gyro_sensor::get_AY() const
{
    return _Accel.y;
}
float gyro_sensor::get_AZ() const
{
    return _Accel.z;
}

float gyro_sensor::get_ASqrt() const
{
    return _aSqrt;
}

// gyro
float gyro_sensor::get_GX() const
{
    return _gyro.x;
}

float gyro_sensor::get_GY() const
{
    return _gyro.y;
}

float gyro_sensor::get_GZ() const
{
    return _gyro.z;
}

String gyro_sensor::get_json() const
{
    return "{\"temp\":\"" + String(_current_temp) + "\",\"psi\":\"" +
           String(_current_pression) + "\"}";
}

void gyro_sensor::start_init_gyro()
{
    _gyro_min.x = _gyro.x;
    _gyro_min.y = _gyro.y;
    _gyro_min.z = _gyro.z;
    _gyro_max.x = _gyro.x;
    _gyro_max.y = _gyro.y;
    _gyro_max.z = _gyro.z;
    _set_gyro_min_max = true;
}
void gyro_sensor::stop_init_gyro()
{
    _set_gyro_min_max = false;
}
void gyro_sensor::reset_init_gyro()
{
    _gyro_min.x = 0.f;
    _gyro_min.y = 0.f;
    _gyro_min.z = 0.f;
    _gyro_max.x = 0.f;
    _gyro_max.y = 0.f;
    _gyro_max.z = 0.f;
    _set_gyro_min_max = false;
}

bool gyro_sensor::_is_moving() const
{
    const float threshold_g = 15.f;
    bool x = _gyro.x > (_gyro_max.x + threshold_g) || _gyro.x < (_gyro_min.x - threshold_g);
    bool y = _gyro.y > (_gyro_max.y + threshold_g) || _gyro.y < (_gyro_min.y - threshold_g);
    bool z = _gyro.z > (_gyro_max.z + threshold_g) || _gyro.z < (_gyro_min.z - threshold_g);
    return x || y || z;
}

bool gyro_sensor::_is_safe() const
{
    const float threshold_a = 0.31f; // +- 18 degree
    return abs(_Accel.x) < threshold_a && abs(_Accel.y) < threshold_a && abs(_Accel.z) < threshold_a;
}
