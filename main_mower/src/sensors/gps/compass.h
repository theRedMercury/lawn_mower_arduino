/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#ifndef COMPAS_H
#define COMPAS_H

#include "../../mower/abs_mower.hpp"
#include "../../tools/tools.hpp"

#define HMC5883L_ADDRESS 0x1E // 0011110b, I2C 7bit address of HMC5883L

// Read / Write
#define HMC5883L_REG_CONF_A 0x00 // 6-5 : SAMPLE / 4-2 : RATE / 1-0 : AXE_MODE(not use)
#define HMC5883L_REG_CONF_B 0x01 // 7-5 : GAIN
#define HMC5883L_REG_MODE 0x02   // 1-0 : MODE

// Read only
#define HMC5883L_REG_OUT_X_M 0x03
#define HMC5883L_REG_OUT_Y_M 0x07
#define HMC5883L_REG_OUT_Z_M 0x05
#define HMC5883L_REG_STATUS 0x09
#define HMC5883L_REG_A 0x0A
#define HMC5883L_REG_B 0x0B
#define HMC5883L_REG_C 0x0C

typedef enum
{
  hmc5883l_mode_idle = 0x02,
  hmc5883l_mode_single = 0x01,
  hmc5883l_mode_continous = 0x00,
} hmc5883l_mode;

typedef enum
{
  hmc5883l_datarate_75hz = 0x06,
  hmc5883l_datarate_30hz = 0x05,
  hmc5883l_datarate_15hz = 0x04,
  hmc5883l_datarate_7_5hz = 0x03,
  hmc5883l_datarate_3hz = 0x02,
  hmc5883l_datarate_1_5hz = 0x01,
  hmc5883l_datarate_0_75_hz = 0x00,
} hmc5883l_data_rate;

typedef enum
{
  hmc5883l_samples_8 = 0x03,
  hmc5883l_samples_4 = 0x02,
  hmc5883l_samples_2 = 0x01,
  hmc5883l_samples_1 = 0x00,
} hmc5883l_samples;

typedef enum
{
  hmc5883l_range_8_1ga = 0x07,
  hmc5883l_range_5_6ga = 0x06,
  hmc5883l_range_4_7ga = 0x05,
  hmc5883l_range_4ga = 0x04,
  hmc5883l_range_2_5ga = 0x03,
  hmc5883l_range_1_9ga = 0x02,
  hmc5883l_range_1_3ga = 0x01,
  hmc5883l_range_0_88ga = 0x00,
} hmc5883l_gain;

class compass_sensor : public abs_mower_class

{
public:
  using abs_mower_class::abs_mower_class;

  void setup();
  void update();

  hmc5883l_mode get_mode() const;
  hmc5883l_data_rate get_rate() const;
  hmc5883l_samples get_sample() const;
  hmc5883l_gain get_gain() const;

  bool set_mode(const hmc5883l_mode mode);
  bool set_rate(const hmc5883l_data_rate rate);
  bool set_sample(const hmc5883l_samples sample);
  bool set_gain(const hmc5883l_gain gain);

  bool is_ready() const;
  unsigned short get_heading_deg() const;

private:
  bool _is_ready_pin();
  void _update_min_max();

  bool _write_register_setting(const unsigned char register_address,
                               const unsigned char mask, const unsigned char pos,
                               const unsigned char conf) const;
  bool _write_register(const unsigned char register_address, const unsigned char byte) const;

  unsigned char _read_register(const unsigned char register_address) const;
  short _read_register_16(const unsigned char register_address) const;

  unsigned short _heading_deg;
  float _gain = 0.92f; // mG/LSb
  bool _is_ready = false;
  XYZ_SHORT _value;
  XYZ_SHORT _min;
  XYZ_SHORT _max;
};

#endif