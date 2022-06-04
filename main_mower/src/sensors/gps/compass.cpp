/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include <Wire.h>

#include "compass.h"
#include "../../mower/mower.hpp"

void compass_sensor::setup()
{
  DEBUG_PRINT("SETUP : ");
  DEBUG_PRINT(class_name);

  _is_ready = true;

  _max.x = __FLT_MIN__;
  _max.y = __FLT_MIN__;
  _max.z = __FLT_MIN__;
  _min.x = __FLT_MAX__;
  _min.y = __FLT_MAX__;
  _min.z = __FLT_MAX__;

  // Check if compass_sensor is alive
  if (_read_register(HMC5883L_REG_A) != 0b01001000 ||
      _read_register(HMC5883L_REG_B) != 0b00110100 ||
      _read_register(HMC5883L_REG_C) != 0b00110011)
  {
    _is_ready = false;
    DEBUG_PRINTLN(" : FAIL");
    return;
  }

  _is_ready &= set_mode(hmc5883l_mode_continous);
  _is_ready &= set_rate(hmc5883l_datarate_30hz);
  _is_ready &= set_sample(hmc5883l_samples_4);
  _is_ready &= set_gain(hmc5883l_range_1_3ga);

  while (!_is_ready_pin())
  {
    _read_register_16(HMC5883L_REG_OUT_X_M);
    _read_register_16(HMC5883L_REG_OUT_Z_M);
    _read_register_16(HMC5883L_REG_OUT_Y_M);
  }
  _is_ready &= _is_ready_pin();

  DEBUG_PRINT("\tM: " + String(get_mode()));
  DEBUG_PRINT("\tR: " + String(get_rate()));
  DEBUG_PRINT("\tS: " + String(get_sample()));
  DEBUG_PRINT("\tG: " + String(get_gain()));
  DEBUG_PRINTLN(_is_ready ? " : DONE" : " : FAIL");
}

void compass_sensor::update()
{
  float heading = 0.f;
  float roll = -mower->gyro.get_AX();
  float pitch = -mower->gyro.get_AY();

  _value.x = _read_register_16(HMC5883L_REG_OUT_X_M);
  _value.z = _read_register_16(HMC5883L_REG_OUT_Z_M);
  _value.y = _read_register_16(HMC5883L_REG_OUT_Y_M);

  DEBUG_PRINT("MAG >");
  DEBUG_PRINT("\tX: " + String(_value.x));
  DEBUG_PRINT("\tY: " + String(_value.y));
  DEBUG_PRINT("\tZ: " + String(_value.z));
  DEBUG_PRINT("\tDeg: " + String(_heading_deg));
  DEBUG_PRINTLN("");

  _update_min_max();

  _value.x = map(_value.x, _min.x, _max.x, -360.f, 360.f);
  _value.y = map(_value.y, _min.y, _max.y, -360.f, 360.f);
  _value.z = map(_value.z, _min.z, _max.z, -360.f, 360.f);

  float x = (float)_value.x * _gain;
  float y = (float)_value.y * _gain;
  float z = (float)_value.z * _gain;

  DEBUG_PRINT("MAG >");
  DEBUG_PRINT("\troll: " + String(roll));
  DEBUG_PRINT("\tpitch: " + String(pitch));
  DEBUG_PRINTLN("");

  if (roll > 0.78 || roll < -0.78 || pitch > 0.78 || pitch < -0.78) // +- 45 deg
  {
    heading = atan2(y, x);
  }
  else
  {
    // Tilt correction
    float cosRoll = cos(roll);
    float sinRoll = sin(roll);
    float cosPitch = cos(pitch);
    float sinPitch = sin(pitch);

    float x_h = x * cosRoll + y * sinPitch - z * cosPitch * sinRoll;
    float y_h = y * cosPitch + z * sinPitch;
    //  Tilt compensation
    // float x_h = x * cosPitch + z * sinPitch;
    // float y_h = x * sinRoll * sinPitch + y * cosRoll - z * sinRoll * cosPitch;
    // float z_h = -x * cosRoll * sinPitch + y * cosRoll - z * sinRoll * cosPitch;
    heading = atan2(x_h, y_h);
  }

  heading += (2.0 + (21.0 / 60.0)) / (180.0 / M_PI); // add '-' if West

  heading += heading < 0.f ? 2.f * PI : 0;
  heading -= heading > 2.f * PI ? 2.f * PI : 0;

  // Convert radians to degrees for readability.
  _heading_deg = abs(round(heading * 180.f / PI));

  DEBUG_PRINT("MAG >");
  DEBUG_PRINT("\tX: " + String(x));
  DEBUG_PRINT("\tY: " + String(y));
  DEBUG_PRINT("\tZ: " + String(z));
  DEBUG_PRINT("\tHea: " + String(heading));
  DEBUG_PRINT("\tDeg: " + String(_heading_deg));
  DEBUG_PRINTLN("");
}

bool compass_sensor::is_ready() const
{
  return this->_is_ready;
}

unsigned short compass_sensor::get_heading_deg() const
{
  return _heading_deg;
}

hmc5883l_mode compass_sensor::get_mode() const
{
  unsigned char value = _read_register(HMC5883L_REG_MODE);
  value &= 0b00000011;
  return (hmc5883l_mode)value;
}

hmc5883l_data_rate compass_sensor::get_rate() const
{
  unsigned char value = _read_register(HMC5883L_REG_CONF_A);
  value &= 0b00011100;
  value >>= 2;
  return (hmc5883l_data_rate)value;
}

hmc5883l_samples compass_sensor::get_sample() const
{
  unsigned char value = _read_register(HMC5883L_REG_CONF_A);
  value &= 0b01100000;
  value >>= 5;
  return (hmc5883l_samples)value;
}

hmc5883l_gain compass_sensor::get_gain() const
{
  return (hmc5883l_gain)((_read_register(HMC5883L_REG_CONF_B) >> 5));
}

bool compass_sensor::set_mode(const hmc5883l_mode mode)
{
  return _write_register_setting(HMC5883L_REG_MODE, 0b11111100, 0, mode);
}

bool compass_sensor::set_rate(const hmc5883l_data_rate rate)
{
  return _write_register_setting(HMC5883L_REG_CONF_A, 0b11100011, 2, rate);
}

bool compass_sensor::set_sample(const hmc5883l_samples sample)
{
  return _write_register_setting(HMC5883L_REG_CONF_A, 0b10011111, 5, sample);
}

bool compass_sensor::set_gain(const hmc5883l_gain gain)
{
  switch (gain)
  {
  case hmc5883l_range_0_88ga:
    _gain = 0.073f;
    break;
  case hmc5883l_range_1_3ga:
    _gain = 0.92f;
    break;
  case hmc5883l_range_1_9ga:
    _gain = 1.22f;
    break;
  case hmc5883l_range_2_5ga:
    _gain = 1.52f;
    break;
  case hmc5883l_range_4ga:
    _gain = 2.27f;
    break;
  case hmc5883l_range_4_7ga:
    _gain = 2.56f;
    break;
  case hmc5883l_range_5_6ga:
    _gain = 3.03f;
    break;
  case hmc5883l_range_8_1ga:
    _gain = 4.35f;
    break;
  default:
    break;
  }
  return _write_register(HMC5883L_REG_CONF_B, gain << 5);
}

bool compass_sensor::_is_ready_pin()
{
  return !!((_read_register(HMC5883L_REG_STATUS)) & 1ULL << (1)) == 0;
}

void compass_sensor::_update_min_max()
{
  _max.x = _value.x > _max.x ? _value.x : _max.x;
  _min.x = _value.x < _min.x ? _value.x : _min.x;

  _min.y = _value.y < _min.y ? _value.y : _min.y;
  _max.y = _value.y > _max.y ? _value.y : _max.y;

  _min.z = _value.z < _min.z ? _value.z : _min.z;
  _max.z = _value.z > _max.z ? _value.z : _max.z;
}

bool compass_sensor::_write_register_setting(const unsigned char register_address, const unsigned char mask, const unsigned char pos, const unsigned char conf) const
{
  unsigned char value = _read_register(register_address);
  value &= mask;
  value |= conf << pos;
  return _write_register(register_address, value);
}

bool compass_sensor::_write_register(const unsigned char register_address, const unsigned char byte) const
{
  Wire.beginTransmission(HMC5883L_ADDRESS);
  Wire.write(register_address);
  Wire.write(byte);
  return Wire.endTransmission() == 0;
}

unsigned char compass_sensor::_read_register(const unsigned char register_address) const
{
  unsigned char value = 0;
  Wire.beginTransmission(HMC5883L_ADDRESS);
  Wire.write(register_address);
  if (Wire.endTransmission() == 0)
  {
    Wire.requestFrom(HMC5883L_ADDRESS, 1);
    while (Wire.available() < 1)
      ;
    value = Wire.read();
    Wire.endTransmission();
    return value;
  }
  return value;
}

inline short compass_sensor::_read_register_16(const unsigned char register_address) const
{
  short value = 0;
  Wire.beginTransmission(HMC5883L_ADDRESS);
  Wire.write(register_address);

  if (Wire.endTransmission() == 0)
  {
    Wire.requestFrom(HMC5883L_ADDRESS, 2);
    while (Wire.available() < 2)
      ;
    unsigned char vha = Wire.read();
    unsigned char vla = Wire.read();
    value = vha << 8 | vla;
    return value;
  }
  return value;
}