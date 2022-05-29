/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "gps.hpp"
#include "../../mower/mower.hpp"

void gps_sensor::setup()
{

    DEBUG_PRINT("SETUP : ");
    DEBUG_PRINT(class_name);
    SERIAL_GPS.begin(9600);

    // https://ozzmaker.com/faq/how-do-i-change-the-baud-rate-on-the-gps-module/
    //  1000 ms = 03E8
    /*unsigned char packet[] = {
        0xB5, // sync char 1
        0x62, // sync char 2
        0x06, // class
        0x08, // id
        0x06, // length
        0x00, // length
        0xE8, // payload
        0x03, // payload
        0x01, // payload
        0x00, // payload
        0x01, // payload
        0x00, // payload
        0x01, // CK_A
        0x39, // CK_B
              //0xB5, 0x62, 0x06, 0x08, 0x00, 0x00, 0x0E, 0x30, 0x0D, 0x0A
    };*/

    // CHANGE BAUDERATE 115200
    /*unsigned char packet[] = {
        0xB5, // sync char 1
        0x62, // sync char 2
        0x06, // class
        0x00, // id
        0x14, // length
        0x00, // length
        0x01, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0xD0, // payload
        0x08, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0xC2, // payload
        0x01, // payload
        0x00, // payload
        0x07, // payload
        0x00, // payload
        0x03, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0xC0, // CK_A
        0x7E, // CK_B
    };*/
    /////////////////////////////////
    // CHANGE BAUDERATE 9600
    unsigned char packet[] = {
        0xB5, // sync char 1
        0x62, // sync char 2
        0x06, // class
        0x00, // id
        0x14, // length
        0x00, // length
        0x01, // payload

        0x00, // payload
        0x00, // payload
        0x00, // payload

        0xD0, // payload
        0x08, // payload
        0x00, // payload
        0x00, // payload

        0x80, // payload
        0x25, // payload
        0x00, // payload
        0x00, // payload
        0x07, // payload
        0x00, // payload
        0x03, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0xA2, // CK_A
        0xB7, // CK_B
    };
    /////////////////////////////////
    // CHANGE BAUDERATE 19200
    /*unsigned char packet[] = {
        0xB5, // sync char 1
        0x62, // sync char 2
        0x06, // class
        0x00, // id
        0x14, // length
        0x00, // length
        0x01, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0xD0, // payload
        0x08, // payload

        0x00, // payload
        0x00, // payload
        0x00, // payload
        0x4B, // payload

        0x00, // payload
        0x00, // payload
        0x07, // payload
        0x00, // payload
        0x03, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0x48, // CK_A
        0x57, // CK_B
    };*/

    for (unsigned char i = 0; i < sizeof(packet); i++)
    {
        SERIAL_GPS.write(packet[i]);
    }
    delay(250);
    // SERIAL_GPS.begin(9600);*/

    delay(100);
    Wire.begin();
    // Put the HMC5883 IC into the correct operating mode
    Wire.beginTransmission(MAG_ADRESS); // open communication with HMC5883
    Wire.write(0x02);                   // select mode register
    Wire.write(0x00);                   // continuous measurement mode
    Wire.endTransmission();

    _is_init_ok = gps_nmea::gps_reset(SERIAL_GPS);
    DEBUG_PRINTLN(_is_init_ok ? " : DONE" : " : FAIL");

    SERIAL_GPS.setTimeout(5);
}

void gps_sensor::update()
{
    if (_is_init_ok)
    {
        update_gps();
    }
    update_mag();
}

void gps_sensor::update_gps()
{
    _is_updated = false;
    _counter_gps_update++;
    if (SERIAL_GPS.available() > 13)
    {
        DEBUG_PRINTLN("READ");
        gps_result _gpsResult;
        gps_nmea::gps_parse(SERIAL_GPS, _gpsResult);
        if (!_gpsResult.gps_success || !_gpsResult.checksum_valid)
        {
            return;
        }
        DEBUG_PRINTLN(_gpsResult.gps_rmc);

        /*
        DEBUG_PRINTLN(_gpsResult.gps_success);
        DEBUG_PRINTLN(_gpsResult.gps_time);
        DEBUG_PRINTLN(_gpsResult.gps_valid);
        DEBUG_PRINTLN(_gpsResult.gps_lat);
        DEBUG_PRINTLN(_gpsResult.gps_ns);
        DEBUG_PRINTLN(_gpsResult.gps_lon);
        DEBUG_PRINTLN(_gpsResult.gps_ew);
        DEBUG_PRINTLN(_gpsResult.gps_spd);
        DEBUG_PRINTLN(_gpsResult.gps_cse);
        DEBUG_PRINTLN(_gpsResult.gps_date);
        DEBUG_PRINTLN(_gpsResult.checksum);
        */

        const float lat = _convert_nmea_to_lat_lon(_gpsResult.gps_lat, _gpsResult.gps_ns);
        const float lon = _convert_nmea_to_lat_lon(_gpsResult.gps_lon, _gpsResult.gps_ew);
        const float speed = atof(_gpsResult.gps_spd);
        const bool success = _gpsResult.gps_success;
        const bool checksum_valid = _gpsResult.checksum_valid;

        _gps_data.success = success;
        _gps_data.checksum_ok = checksum_valid;

        if ((_gps_data.lat != lat || _gps_data.lon != lon || _gps_data.speed != speed || _gps_data.success != success) &&
            (_gps_data.lat == 0.f && _gps_data.lon == 0.f && lat != 0.f && lon != 0.f))
        {
            _gps_data.lat = _convert_nmea_to_lat_lon(_gpsResult.gps_lat, _gpsResult.gps_ns);
            _gps_data.lon = _convert_nmea_to_lat_lon(_gpsResult.gps_lon, _gpsResult.gps_ew);
            _gps_data.speed = atof(_gpsResult.gps_spd);

            _is_updated = true;
            _counter_gps_update = 0;
        }

        if (!_is_updated && _counter_gps_update == 255)
        {
            _is_updated = true;
            _counter_gps_update = 0;
        }

        //_hundredths = gps_parse_unsigned_int(_gpsResult.gps_time + 7, 2);
        if (strncmp("$GNRMC", _gpsResult.gps_rmc, 6) == 0)
        {
            unsigned char Hour = gps_nmea::gps_parse_unsigned_int(_gpsResult.gps_time, 2, mower->time.get_hour() - 2, 0, 24) + 2; // + 2 = (GMT +1) + (Summer +1)
            unsigned char Minute = gps_nmea::gps_parse_unsigned_int(_gpsResult.gps_time + 2, 2, mower->time.get_minute());
            unsigned char Second = gps_nmea::gps_parse_unsigned_int(_gpsResult.gps_time + 4, 2, mower->time.get_second());
            unsigned char Day = gps_nmea::gps_parse_unsigned_int(_gpsResult.gps_date, 2, mower->time.get_day(), 1, 31);
            unsigned char Month = gps_nmea::gps_parse_unsigned_int(_gpsResult.gps_date + 2, 2, mower->time.get_month(), 1, 12);
            unsigned short Year = gps_nmea::gps_parse_unsigned_int(_gpsResult.gps_date + 4, 2, mower->time.get_year(), 0, 99);

            DEBUG_PRINT("HOUR >\t");
            DEBUG_PRINT(DECIMAL_TIME_STR(Hour));
            DEBUG_PRINT(':');
            DEBUG_PRINT(DECIMAL_TIME_STR(Minute));
            DEBUG_PRINT(':');
            DEBUG_PRINT(DECIMAL_TIME_STR(Second));
            DEBUG_PRINT("  --  ");
            DEBUG_PRINT(DECIMAL_TIME_STR(Day));
            DEBUG_PRINT("/");
            DEBUG_PRINT(DECIMAL_TIME_STR(Month));
            DEBUG_PRINT("/");
            DEBUG_PRINT(Year);
            DEBUG_PRINTLN("");

            mower->time.set_time(Hour, Minute, Second, Day, Month, Year);
            _time_is_valid = true;
        }

        DEBUG_PRINT("GPS >\t");
        DEBUG_PRINT(mower->time.get_hour());
        DEBUG_PRINT(':');
        DEBUG_PRINT(mower->time.get_minute());
        DEBUG_PRINT(':');
        DEBUG_PRINTLN(mower->time.get_second());

        DEBUG_PRINT("\t");
        DEBUG_PRINT(mower->time.get_day());
        DEBUG_PRINT("/");
        DEBUG_PRINT(mower->time.get_month());
        DEBUG_PRINT("/");
        DEBUG_PRINTLN(mower->time.get_year());
        DEBUG_PRINT("\t");
        DEBUG_PRINTLN(static_cast<int>(mower->time.get_day_week_num()));
        DEBUG_PRINTLN(_gpsResult.gps_date);
    }

    /*DEBUG_PRINT("GPS >\t");
    DEBUG_PRINT(Hour);
    DEBUG_PRINT(':');
    DEBUG_PRINT(Minute);
    DEBUG_PRINT(':');
    DEBUG_PRINTLN(Second);

    DEBUG_PRINT("\t");
    DEBUG_PRINT(Day);
    DEBUG_PRINT("/");
    DEBUG_PRINT(Month);
    DEBUG_PRINT("/");
    DEBUG_PRINTLN(Year);*/
    DEBUG_PRINT("\t");
    DEBUG_PRINT(_gps_data.lat);
    DEBUG_PRINT(" / ");
    DEBUG_PRINT(_gps_data.lon);
    DEBUG_PRINT(" - ");
    DEBUG_PRINT(_gps_data.speed);
    DEBUG_PRINT(" > ");
    // DEBUG_PRINT(String(dayStr(weekday())));
    DEBUG_PRINTLN(" ");
}

void gps_sensor::update_mag()
{
    // Tell the HMC5883 where to begin reading data
    Wire.beginTransmission(MAG_ADRESS);
    Wire.write(0x03); // select register 3, X MSB register
    Wire.endTransmission();

    // Read data from each axis, 2 registers per axis
    Wire.requestFrom(MAG_ADRESS, 6);
    if (Wire.available() >= 6)
    {
        _magne.x = Wire.read() << 8; // X msb
        _magne.x |= Wire.read();     // X lsb
        _magne.z = Wire.read() << 8; // Z msb
        _magne.z |= Wire.read();     // Z lsb
        _magne.y = Wire.read() << 8; // Y msb
        _magne.y |= Wire.read();     // Y lsb

        float heading = atan2(_magne.y, _magne.x) + 1.115192f;
        // Once you have your heading, you must then add your 'Declination Angle', which is the 'Error' of the magnetic field in your location.
        // Find yours here: http://www.magnetic-declination.com/
        // Mine is: -13* 2' W, which is ~13 Degrees, or (which we need) 0.22 radians
        // If you cannot find your Declination, comment out these two lines, your compass will be slightly off.
        // float declinationAngle = 0.22;

        // Correct for when signs are reversed.
        if (heading < 0.f)
        {
            heading += 2.f * PI;
        }

        // Check for wrap due to addition of declination.
        if (heading > 2.f * PI)
        {
            heading -= 2.f * PI;
        }

        // Convert radians to degrees for readability.
        _heading_deg = static_cast<unsigned short>(abs(round(360.f - (heading * 180.f / PI))));
        _is_ready = true;
    }

    DEBUG_PRINT("MAG >")
    DEBUG_PRINT("\tX: " + String(_magne.x));
    DEBUG_PRINT("\tY: " + String(_magne.y));
    DEBUG_PRINT("\tZ: " + String(_magne.z));
    DEBUG_PRINT("\tDeg: " + String(get_heading_deg()));
    DEBUG_PRINTLN("");
}

const gps_data *gps_sensor::get_gps_data() const
{
    return &_gps_data;
}

const unsigned short gps_sensor::get_heading_deg() const
{
    return _heading_deg;
}

const bool gps_sensor::is_ready() const
{
    return _is_ready && _is_init_ok;
}

const bool gps_sensor::is_time_valid() const
{
    return _time_is_valid;
}

const bool gps_sensor::is_lat_lon_valid() const
{
    return _gps_data.lat != 0.0 && _gps_data.lon != 0.0;
}

const bool gps_sensor::is_updated() const
{
    return _is_updated;
}

const String gps_sensor::get_json() const
{
    return "{\"lat\":\"" + String(_gps_data.lat, 7) + "\",\"lon\":\"" +
           String(_gps_data.lon, 7) + "\",\"speed\":\"" + String(_gps_data.speed) + "\",\"hed\":\"" + String(_heading_deg) + "\"}";
}

const float gps_sensor::_convert_nmea_to_lat_lon(const char *nmeaPos, char quadrant)
{
    float v = 0;
    if (strlen(nmeaPos) > 5)
    {
        char integerPart[3 + 1];
        int digitCount = (nmeaPos[4] == '.' ? 2 : 3);
        memcpy(integerPart, nmeaPos, digitCount);
        integerPart[digitCount] = 0;
        nmeaPos += digitCount;
        v = atoi(integerPart) + atof(nmeaPos) / 60.;
        if (quadrant == 'W' || quadrant == 'S')
            v = -v;
    }
    return v;
}
