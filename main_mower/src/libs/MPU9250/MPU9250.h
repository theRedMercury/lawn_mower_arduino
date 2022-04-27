#ifndef MPU9250_H
#define MPU9250_H
#include <Arduino.h>
#include <Wire.h>
#include <math.h>

#define AK8963_ADDRESS 0x0C
#define AK8963_RA_HXL 0x03
#define AK8963_RA_CNTL1 0x0A
#define AK8963_RA_ASAX 0x10

#define MPU9250_ADDR_ACCELCONFIG 0x1C
#define MPU9250_ADDR_INT_PIN_CFG 0x37
#define MPU9250_ADDR_ACCEL_XOUT_H 0x3B
#define MPU9250_ADDR_GYRO_XOUT_H 0x43
#define MPU9250_ADDR_PWR_MGMT_1 0x6B
#define MPU9250_ADDR_WHOAMI 0x75

#define MPU9250_ADDRESS_AD0_LOW 0x68
#define MPU9250_ADDRESS_AD0_HIGH 0x69

#define ACC_FULL_SCALE_2_G 0x00
#define ACC_FULL_SCALE_4_G 0x08
#define ACC_FULL_SCALE_8_G 0x10
#define ACC_FULL_SCALE_16_G 0x18

#define GYRO_FULL_SCALE_250_DPS 0x00
#define GYRO_FULL_SCALE_500_DPS 0x08
#define GYRO_FULL_SCALE_1000_DPS 0x10
#define GYRO_FULL_SCALE_2000_DPS 0x18

#define MAG_MODE_POWERDOWN 0x0
#define MAG_MODE_SINGLE 0x1
#define MAG_MODE_CONTINUOUS_8HZ 0x2
#define MAG_MODE_EXTERNAL 0x4
#define MAG_MODE_CONTINUOUS_100HZ 0x6
#define MAG_MODE_SELFTEST 0x8
#define MAG_MODE_FUSEROM 0xF

#define MPU9250_BUFF_LEN_ACCEL 6
#define MPU9250_BUFF_LEN_GYRO 6
#define MPU9250_BUFF_LEN_MAG 7

class MPU9250
{
public:
  const unsigned char address;
  short magXOffset, magYOffset, magZOffset;
  unsigned char accelBuff[MPU9250_BUFF_LEN_ACCEL];
  unsigned char gyroBuff[MPU9250_BUFF_LEN_GYRO];
  unsigned char magBuff[MPU9250_BUFF_LEN_MAG];

  MPU9250(unsigned char address = MPU9250_ADDRESS_AD0_LOW);
  void setWire(TwoWire *wire);
  unsigned char readId(unsigned char *id);

  void beginAccel(unsigned char mode = ACC_FULL_SCALE_16_G);
  unsigned char accelUpdate();
  float accelX();
  float accelY();
  float accelZ();
  float accelSqrt();

  void beginGyro(unsigned char mode = GYRO_FULL_SCALE_2000_DPS);
  unsigned char gyroUpdate();
  float gyroX();
  float gyroY();
  float gyroZ();

  void beginMag(unsigned char mode = MAG_MODE_CONTINUOUS_8HZ);
  void magSetMode(unsigned char mode);
  unsigned char magUpdate();
  float magX();
  float magY();
  float magZ();
  float magHorizDirection();

private:
  TwoWire *myWire;
  float accelRange;
  float gyroRange;
  unsigned char magXAdjust, magYAdjust, magZAdjust;
  void beginWireIfNull();
  float accelGet(unsigned char highIndex, unsigned char lowIndex);
  float gyroGet(unsigned char highIndex, unsigned char lowIndex);
  short magGet(unsigned char highIndex, unsigned char lowIndex);
  void magEnableSlaveMode();
  void magReadAdjustValues();
  void magWakeup();
  unsigned char i2cRead(unsigned char Address, unsigned char Register, unsigned char Nbytes, unsigned char *Data);
  unsigned char i2cWriteByte(unsigned char Address, unsigned char Register, unsigned char Data);
};

#endif
