/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "gps_nmea.h"

const unsigned int gps_nmea::gps_parse_unsigned_int(const char *s, unsigned char len, unsigned int default_int, unsigned int min_int, unsigned int max_int)
{
  unsigned int r = 0;
  while (len--)
  {
    r = 10 * r + *s++ - '0';
  }
  if (r < min_int || r > max_int)
  {
    return default_int;
  }
  return r;
}

const bool gps_nmea::gps_reset(HardwareSerial &ser)
{
  char temp;
  unsigned int counter_loop = 0;
  do
  {
    counter_loop++;
    if (ser.available() > 0)
      temp = ser.read();

    if (counter_loop > 100000) // Timeout
      return false;

  } while (temp != '$');

  return true;
}

const void gps_nmea::gps_parse(HardwareSerial &ser, gps_result &result)
{
  char rmc[NMEA_SIZE_PACKET] = {'\0'};
  unsigned int size_packet = 0;
  result.gps_success = false;
  result.checksum_valid = false;

  String gps_data = ser.readStringUntil('\n');
  if (!gps_data.startsWith("$GNRMC,")) //$GN // Ingore $GNVTG & others
  {
    ser.flush();
    return;
  }

  // CLEAN & get last $ if multiple in one line
  unsigned int counterDollar = 0;
  unsigned int lastIndex = 0;
  for (unsigned int i = 0; i < gps_data.length(); i++)
  {
    if (gps_data[i] == '$')
    {
      counterDollar += 1;
      lastIndex = i;
    }
  }
  if (counterDollar > 1)
  {
    gps_data = gps_data.substring(lastIndex, gps_data.length());
  }
  //------------------------------------------------------

  size_packet = gps_data.length();
  gps_data.toCharArray(rmc, NMEA_SIZE_PACKET);

  if (gps_data[size_packet - 3] == '*')
  {
    String hexV = String(gps_data[size_packet - 2]) + String(gps_data[size_packet - 1]);
    result.checksum = strtoul(hexV.c_str(), 0, 16);
    result.checksum_valid = _nmea0183_checksum_valid(rmc, result.checksum);
  }
  result.gps_success = true;

  strcpy(result.gps_rmc, rmc);

  _parse_rmc_time(rmc, result.gps_time, size_packet);
  _parse_rmc_valid(rmc, &result.gps_valid, size_packet);
  _parse_rmc_lat(rmc, result.gps_lat, size_packet);
  _parse_rmc_NS(rmc, &result.gps_ns, size_packet);
  _parse_rmc_lon(rmc, result.gps_lon, size_packet);
  _parse_rmc_ew(rmc, &result.gps_ew, size_packet);
  _parse_rmc_spd(rmc, result.gps_spd, size_packet);
  _parse_rmc_cse(rmc, result.gps_cse, size_packet);
  _parse_rmc_date(rmc, result.gps_date, size_packet);
}

void gps_nmea::_generic_parse_nmea(char gps_nmea[], char *gps_out, unsigned int pos_coma, const unsigned int size)
{
  unsigned int c = 0;
  unsigned int i = 0;
  unsigned int limit = 0;
  for (i = 0; i < pos_coma; i++)
  {
    while (gps_nmea[c] != ',' && limit < size)
    {
      c++;
      limit++;
    }
    c++;
  }
  limit = 0;
  i = 0;
  int posStart = c;
  while (gps_nmea[c] != ',' && limit < size)
  {
    c++;
    i++;
    limit++;
  }
  memcpy(gps_out, &gps_nmea[posStart], i);
}

void gps_nmea::_parse_rmc_time(char *gps_str, char *gps_time, const unsigned int size)
{
  unsigned int c = 0;
  unsigned int i = 0;
  for (; c < size; c++)
  {
    if (gps_str[c] == ',')
      break;
  }
  c++;
  while (gps_str[c] != ',' && i < 10)
  {
    gps_time[i] = gps_str[c];
    c++;
    i++;
  }
}

void gps_nmea::_parse_rmc_valid(char *gps_str, char *gpsValid, const unsigned int size)
{
  _generic_parse_nmea(gps_str, gpsValid, 2, size);
}

void gps_nmea::_parse_rmc_lat(char *gps_str, char *gpsLat, const unsigned int size)
{
  _generic_parse_nmea(gps_str, gpsLat, 3, size);
}

void gps_nmea::_parse_rmc_NS(char *gps_str, char *gpsNS, const unsigned int size)
{
  _generic_parse_nmea(gps_str, gpsNS, 4, size);
}

void gps_nmea::_parse_rmc_lon(char *gps_str, char *gpsLon, const unsigned int size)
{
  _generic_parse_nmea(gps_str, gpsLon, 5, size);
}

void gps_nmea::_parse_rmc_ew(char *gps_str, char *gpsEW, const unsigned int size)
{

  _generic_parse_nmea(gps_str, gpsEW, 6, size);
}

void gps_nmea::_parse_rmc_spd(char *gps_str, char *gpsSpeed, const unsigned int size)
{
  _generic_parse_nmea(gps_str, gpsSpeed, 7, size);
}

void gps_nmea::_parse_rmc_cse(char *gps_str, char *gpsCSE, const unsigned int size)
{
  _generic_parse_nmea(gps_str, gpsCSE, 8, size);
}

void gps_nmea::_parse_rmc_date(char *gps_str, char *gpsDate, const unsigned int size)
{
  _generic_parse_nmea(gps_str, gpsDate, 9, size);
}

bool gps_nmea::_nmea0183_checksum_valid(char *gps_data, int checksum)
{
  int crc = -1;
  // ignore the first $ sign,  no checksum in sentence
  for (unsigned int i = 1; i < strlen(gps_data); i++)
  { // removed the - 3 because no cksum is present
    crc ^= gps_data[i];
  }
  return crc == checksum;
}
