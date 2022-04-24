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

#define MAG_ADRESS 0x1E // 0011110b, I2C 7bit address of HMC5883

struct PACKING gps_data
{
    bool success = false;
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

    void update_gps();
    void update_mag();
    const gps_data *get_gps_data() const;
    const uint16_t get_heading_deg() const;

    const bool is_ready() const;
    const bool is_time_valid() const;
    const bool is_lat_lon_valid() const;
    const bool is_updated() const;

    const String get_json() const;

private:
    const float _convert_nmea_to_lat_lon(const char *nmeaPos, char quadrant);

    gps_data _gps_data;
    unsigned long _last_gps_update = 0;
    XYZ_INT _magne;
    float _heading_deg = 0;
    bool _is_init_ok = false;
    bool _is_ready = false;
    bool _time_is_valid = false;
    bool _is_updated = false;
    uint8_t _counter_gps_update = 0;
};

#endif