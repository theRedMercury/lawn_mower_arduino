/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */
#ifndef LCD_GPS_H
#define LCD_GPS_H

#include "../../mower/mower.hpp"

enum lcd_menu_gps : char
{
    GPS_COORD = 0,
    GPS_SPEED = 1,
    GPS_TIME = 2,
    GPS_HEADING = 3,
    GPS_STATUS = 4
};
lcd_menu_gps _current_gps_menu;

void Lcd_navigation_menu_gps(mower_manager *mower)
{
    mower->lcd.process_sub_menu_input(_current_gps_menu, 4);
    mower->lcd.get_screen().setCursor(5, 0);

    switch (_current_gps_menu)
    {
    case GPS_COORD:
        mower->lcd.get_screen().print("> COORD");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        mower->lcd.get_screen().print(mower->gps.get_gps_data()->lat);
        mower->lcd.get_screen().print(" - ");
        mower->lcd.get_screen().print(mower->gps.get_gps_data()->lon);
        break;

    case GPS_SPEED:
        mower->lcd.get_screen().print("> SPEED");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        mower->lcd.get_screen().print(mower->gps.get_gps_data()->speed);
        break;

    case GPS_TIME:
        mower->lcd.get_screen().print("> TIME");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        mower->lcd.get_screen().print(DECIMAL_TIME_STR(mower->time.get_hour()));
        mower->lcd.get_screen().print(':');
        mower->lcd.get_screen().print(DECIMAL_TIME_STR(mower->time.get_minute()));
        // mower->lcd.get_screen().print(':'); // Not enough place on screen
        // mower->lcd.get_screen().print(DECIMAL_TIME_STR(mower->time.get_second()));
        mower->lcd.get_screen().setCursor(8, 1);
        mower->lcd.get_screen().print(DECIMAL_TIME_STR(mower->time.get_day()));
        mower->lcd.get_screen().print('/');
        mower->lcd.get_screen().print(DECIMAL_TIME_STR(mower->time.get_month()));
        mower->lcd.get_screen().print('/');
        mower->lcd.get_screen().print(constrain(mower->time.get_year() - 2000, 0, 99)); // Show only last two number
        break;

    case GPS_HEADING:
        mower->lcd.get_screen().print("> HEADING");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        mower->lcd.get_screen().print(mower->gps.get_heading_deg());
        break;

    case GPS_STATUS:
        mower->lcd.get_screen().print("> STATUS");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        mower->lcd.get_screen().print(mower->gps.get_gps_data()->success ? "Success" : "Not OK");
        mower->lcd.get_screen().setCursor(8, 1);
        mower->lcd.get_screen().print(mower->gps.get_gps_data()->checksum_ok ? "Valid" : "Invalid");
        break;

    default:
        break;
    }
}

#endif