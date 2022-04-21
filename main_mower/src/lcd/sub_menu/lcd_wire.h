/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */
#ifndef LCD_WIRE_H
#define LCD_WIRE_H

#include "../../mower/mower.hpp"

enum lcd_menu_wire
{
    WIRE_STATUS = 0
};
lcd_menu_wire _currentWireMenu;

void Lcd_navigation_menu_wire(mower_manager *mower)
{
    mower->lcd.process_sub_menu_input(_currentWireMenu, 0);
    mower->lcd.get_screen().setCursor(5, 0);

    // SHOW MENU
    if (_currentWireMenu == WIRE_STATUS)
    {
        mower->lcd.get_screen().print("> STATUS ");
        mower->lcd.get_screen().print(mower->perim.is_inside());
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        if (mower->perim.get_magnitude() >= 0)
        {
            mower->lcd.get_screen().setCursor(1, 1);
        }
        mower->lcd.get_screen().print(mower->perim.get_magnitude());
        mower->lcd.get_screen().setCursor(8, 1);
        mower->lcd.get_screen().print("> ");
        mower->lcd.get_screen().print(mower->perim.get_filter_quality());
    }
}

#endif