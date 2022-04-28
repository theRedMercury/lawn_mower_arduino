/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#ifndef MOWER_H
#define MOWER_H

#ifndef __AVR__
#error Arduino Mega or Mega 2560 support only
#endif

#include <HardwareSerial.h>
///////////////////////////////////////////////////////
// PRINT SERIAL MODE
///////////////////////////////////////////////////////
//#define DEBUG_PRINTER Serial // Comment if RELEASE
//#define DEBUG_MODE 0         // Comment if RELEASE
//#define DEBUG_SPEED_MODE 0   // Comment if RELEASE
//#define DEBUG_SIMULATE_WIRE 0   // Comment if RELEASE
#define DEBUG_IS_TIME_TO_MOWN 0 // Comment if RELEASE

#ifdef DEBUG_MODE
#define DEBUG_PRINT(...)                  \
    {                                     \
        DEBUG_PRINTER.print(__VA_ARGS__); \
    }
#define DEBUG_PRINTLN(...)                  \
    {                                       \
        DEBUG_PRINTER.println(__VA_ARGS__); \
    }
#else
#define DEBUG_PRINT(...) \
    {                    \
    }
#define DEBUG_PRINTLN(...) \
    {                      \
    }
#endif

#define BOOL_STR(b) ((b) ? "yes" : "no")

enum class mower_status
{
    READY,
    WAITING,
    RUNNING,
    LEAVING_STATION,
    RETURN_STATION,
    CHARGING,
    TESTING_NAV,
    TESTING_NAV_02,
    ERROR_INIT,
    ERROR_LOST_WIRE,
    ERROR_POWER,
    ERROR
};

///////////////////////////////////////////////////////
// MOWER
///////////////////////////////////////////////////////
#include "../lcd/lcd.hpp"
#include "../motor/motor.hpp"
#include "../motor/motor_blade.hpp"
#include "../navigation/navigation.hpp"
#include "../navigation/scheduler.hpp"
#include "../perimeter/perimeter.hpp"
#include "../sensors/elec.hpp"
#include "../sensors/gps/gps.hpp"
#include "../sensors/gyro.hpp"
#include "../sensors/sonar.hpp"
#include "../sensors/rain.hpp"
#include "../tools/time_manager.hpp"
#include "../wifi/wifi.hpp"

class mower_manager
{
public:
    mower_manager() {}
    mower_manager(const mower_manager &) = delete;
    mower_manager(mower_manager &&) = delete;
    mower_manager &operator=(const mower_manager &) = delete;
    mower_manager &operator=(mower_manager &&) = delete;

    void setup();
    void update();
    void set_error(const mower_status error = mower_status::ERROR);

    const char *get_current_status_str() const;
    const mower_status get_current_status() const;
    void set_current_status(const mower_status new_status);

    time_manager time;
    // ALL CONTROLLERS / SENSORS
    lcd_control lcd{*this, "LCD"};
    motor_control motor{*this, "MOTOR"};
    motor_control_blade motor_blade{*this, "MOTOR BLADE"};
    elec_sensor elec{*this, "ELEC"};
    gps_sensor gps{*this, "GPS"};
    gyro_sensor gyro{*this, "GYRO"};
    navigation nav{*this, "NAVIGATION"};
    perimeter perim{*this, "PERIMETER"};
    sonar_sensor sonar{*this, "SONAR"};
    scheduler schedul{*this, "SCHEDULER"};
    rain_sensor rain{*this, "RAIN"};
    wifi_control wifi{*this, "WIFI"};

private:
    mower_status _current_status = mower_status::WAITING;
};
#endif