/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#ifndef GPS_NMEA_h
#define GPS_NMEA_h

#include <Arduino.h>

#include "../../tools/tools.hpp"

#define NMEA_SIZE_PACKET 150

struct PACKING gps_result
{
  bool gps_success = false;
  char gps_rmc[NMEA_SIZE_PACKET] = {0};
  char gps_time[10] = {0};
  char gps_valid = 0;
  char gps_lat[15] = {0};
  char gps_ns = 0;
  char gps_lon[15] = {0};
  char gps_ew = 0;
  char gps_spd[10] = {0};
  char gps_cse[10] = {0};
  char gps_date[10] = {0};
  int checksum = 0;
  bool checksum_valid = false;
};

class gps_nmea
{
public:
  static unsigned int gps_parse_unsigned_int(const char *s, unsigned char len, unsigned int defaultInt = 0, unsigned int minInt = 0, unsigned int maxInt = 60);
  static bool gps_reset(HardwareSerial &ser);
  static void gps_parse(HardwareSerial &ser, gps_result &result);

private:
  static void _generic_parse_nmea(char gps_nmea[], char *gps_out, unsigned int pos_coma, const unsigned int size);
  static void _parse_rmc_time(char *gps_str, char *gps_time, const unsigned int size);
  static void _parse_rmc_valid(char *gps_str, char *gpsValid, const unsigned int size);
  static void _parse_rmc_lat(char *gps_str, char *gpsLat, const unsigned int size);
  static void _parse_rmc_NS(char *gps_str, char *gpsNS, const unsigned int size);
  static void _parse_rmc_lon(char *gps_str, char *gpsLon, const unsigned int size);
  static void _parse_rmc_ew(char *gps_str, char *gpsEW, const unsigned int size);
  static void _parse_rmc_spd(char *gps_str, char *gpsSpeed, const unsigned int size);
  static void _parse_rmc_cse(char *gps_str, char *gpsCSE, const unsigned int size);
  static void _parse_rmc_date(char *gps_str, char *gpsDate, const unsigned int size);
  static bool _nmea0183_checksum_valid(char *gps_data, const unsigned int size, const int checksum);

  gps_nmea();
};

#endif
