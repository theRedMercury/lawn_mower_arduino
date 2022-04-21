/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */
#ifndef LCD_WIFI_H
#define LCD_WIFI_H

#include "../../mower/mower.hpp"

enum lcd_menu_wifi
{
    WIFI_STATUS = 0,
    WIFI_LAST_MSG = 1
};
lcd_menu_wifi _currentWifiMenu;

void Lcd_navigation_menu_wifi(mower_manager *mower)
{
    mower->lcd.process_sub_menu_input(_currentWifiMenu, 1);
    mower->lcd.get_screen().setCursor(5, 0);

    // SHOW MENU
    switch (_currentWifiMenu)
    {
    case WIFI_STATUS:
        mower->lcd.get_screen().print("> STATUS ");
        mower->lcd.get_screen().print(mower->wifi.is_ready());
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().print(mower->wifi.is_ready() ? "Connected" : "Not ready");
        break;

    case WIFI_LAST_MSG:
        mower->lcd.get_screen().print("> MSG ");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        mower->lcd.get_screen().print(mower->wifi.get_last_msg());
        break;
    default:
        break;
    }
}

#endif