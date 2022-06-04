/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#ifndef GPS_H
#define GPS_H

#include "../../mower/abs_mower.hpp"
#include "../../tools/tools.hpp"

#include <HardwareSerial.h>
#include "gps_nmea.h"

#define SERIAL_GPS Serial2

struct PACKING gps_data
{
    bool success = false;
    bool checksum_ok = false;
    float lat = 0.f;
    float lon = 0.f;
    float speed = 0.f;
};

class gps_sensor : public abs_mower_class
{
public:
    using abs_mower_class::abs_mower_class;

    void setup();
    void update();

    void retry_init();

    const gps_data *get_gps_data() const;
    unsigned short get_heading_deg() const;

    bool is_ready() const;
    bool is_time_valid() const;
    bool is_lat_lon_valid() const;
    bool is_updated() const;

    String get_json() const;

private:
    float _convert_nmea_to_lat_lon(const char *nmeaPos, char quadrant);

    gps_data _gps_data;
    unsigned long _last_gps_update = 0;
    bool _is_ready = false;
    bool _time_is_valid = false;
    bool _is_updated = false;
    unsigned char _counter_gps_update = 0;
};

#endif