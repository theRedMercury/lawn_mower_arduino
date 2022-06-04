/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */
#ifndef LCD_SENSOR_H
#define LCD_SENSOR_H

#include "../../mower/mower.hpp"

enum lcd_menu_sensor : char
{
    SENSOR_SONAR = 0,
    SENSOR_RAIN = 1,
    SENSOR_TEMP = 2,
    SENSOR_GYRO_AA = 3,
    SENSOR_GYRO_A = 4,
    SENSOR_GYRO_G = 5,
    SENSOR_GYRO_A_SQRT = 6,
    SENSOR_GYRO_SAFE = 7,
    SENSOR_ELEC = 8,
    // SENSOR_BUMPER = 4
};
lcd_menu_sensor _currentSensorMenu;

void Lcd_navigation_menu_sensor(mower_manager *mower)
{
    mower->lcd.process_sub_menu_input(_currentSensorMenu, 8);
    mower->lcd.get_screen().setCursor(7, 0);
    switch (_currentSensorMenu)
    {
    case SENSOR_SONAR:
        mower->lcd.get_screen().print("> SONAR");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        mower->lcd.get_screen().print(mower->sonar.get_left());
        mower->lcd.get_screen().setCursor(6, 1);
        mower->lcd.get_screen().print(mower->sonar.get_center());
        mower->lcd.get_screen().setCursor(12, 1);
        mower->lcd.get_screen().print(mower->sonar.get_right());
        break;

    case SENSOR_RAIN:
        mower->lcd.get_screen().print("> RAIN");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        mower->lcd.get_screen().print(mower->rain.get_value());
        mower->lcd.get_screen().setCursor(7, 1);
        mower->lcd.get_screen().print("> ");
        mower->lcd.get_screen().print(mower->rain.is_rainning() ? "True" : "False");
        break;

    case SENSOR_TEMP:
        mower->lcd.get_screen().print("> TEMP");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        mower->lcd.get_screen().print(mower->gyro.get_temp());
        mower->lcd.get_screen().print(" C");
        mower->lcd.get_screen().setCursor(8, 1);
        mower->lcd.get_screen().print(mower->gyro.get_pression());
        mower->lcd.get_screen().print(" mBar");
        break;

    case SENSOR_GYRO_AA:
        mower->lcd.get_screen().print("> GYRO a");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        mower->lcd.get_screen().print(mower->gyro.get_ax(), 1);
        mower->lcd.get_screen().setCursor(5, 1);
        mower->lcd.get_screen().print(mower->gyro.get_ay(), 1);
        mower->lcd.get_screen().setCursor(10, 1);
        mower->lcd.get_screen().print(mower->gyro.get_az(), 1);
        break;

    case SENSOR_GYRO_A:
        mower->lcd.get_screen().print("> GYRO A");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        mower->lcd.get_screen().print(mower->gyro.get_AX(), 1);
        mower->lcd.get_screen().setCursor(5, 1);
        mower->lcd.get_screen().print(mower->gyro.get_AY(), 1);
        mower->lcd.get_screen().setCursor(10, 1);
        mower->lcd.get_screen().print(mower->gyro.get_AZ(), 1);
        break;

    case SENSOR_GYRO_G:
        mower->lcd.get_screen().print("> GYRO Gy");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        mower->lcd.get_screen().print(mower->gyro.get_GX(), 1);
        mower->lcd.get_screen().setCursor(5, 1);
        mower->lcd.get_screen().print(mower->gyro.get_GY(), 1);
        mower->lcd.get_screen().setCursor(10, 1);
        mower->lcd.get_screen().print(mower->gyro.get_GZ(), 1);
        break;

    case SENSOR_GYRO_A_SQRT:
        mower->lcd.get_screen().print("> GYRO AS");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        mower->lcd.get_screen().print(mower->gyro.get_ASqrt());
        mower->lcd.get_screen().setCursor(7, 1);
        mower->lcd.get_screen().print(mower->gyro.have_shock());
        break;

    case SENSOR_GYRO_SAFE:
        mower->lcd.get_screen().print("> GYRO");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        mower->lcd.get_screen().print("> Safe ");
        mower->lcd.get_screen().print(mower->gyro.in_safe_status() ? "yes" : "no");
        mower->lcd.get_screen().setCursor(12, 1);
        mower->lcd.get_screen().print(mower->gyro.is_moving() ? "yes" : "no");
        break;

    case SENSOR_ELEC:
        mower->lcd.get_screen().print("> ELEC");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        mower->lcd.get_screen().print(mower->elec.get_curent_volt());
        mower->lcd.get_screen().print("V ");
        mower->lcd.get_screen().print(mower->elec.is_battery_low() ? "LOW" : "OK");
        mower->lcd.get_screen().setCursor(11, 1);
        mower->lcd.get_screen().print(mower->elec.get_curent_amp());
        mower->lcd.get_screen().print("A");
        break;
    default:
        break;
    }
};

#endif