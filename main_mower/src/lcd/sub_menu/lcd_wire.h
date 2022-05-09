/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */
#ifndef LCD_WIRE_H
#define LCD_WIRE_H

#include "../../mower/mower.hpp"

enum lcd_menu_wire : char
{
    WIRE_STATUS = 0,
    WIRE_SET_SIMULATE = 1
};
lcd_menu_wire _currentWireMenu;

void Lcd_navigation_menu_wire(mower_manager *mower)
{
    mower->lcd.process_sub_menu_input(_currentWireMenu, 1);
    mower->lcd.get_screen().setCursor(5, 0);

    // SHOW MENU
    switch (_currentWireMenu)
    {
    case WIRE_STATUS:
        mower->lcd.get_screen().print("> STATUS ");
        mower->lcd.get_screen().print(mower->perim.is_inside() ? "IN" : "OU");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        if (mower->perim.get_magnitude() >= 0)
        {
            mower->lcd.get_screen().setCursor(1, 1);
        }
        mower->lcd.get_screen().print(mower->perim.get_magnitude());
        mower->lcd.get_screen().setCursor(7, 1);
        mower->lcd.get_screen().print(mower->perim.get_filter_quality());
        mower->lcd.get_screen().setCursor(14, 1);
        mower->lcd.get_screen().print(mower->perim.is_signal_timed_out() ? "TO" : "OK");
        break;

    case WIRE_SET_SIMULATE:
        mower->lcd.get_screen().print("> SET SIMU ");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().print(mower->perim.get_simulate_is_inside() ? "SIMULATE WIRE" : "NOT SIMULATE");

        if (mower->lcd.get_current_input() == current_flag_input::INPUT_OK)
        {
            mower->perim.set_simulate_is_inside(!mower->perim.get_simulate_is_inside());
        }
        break;
    default:
        break;
    }
}

#endif