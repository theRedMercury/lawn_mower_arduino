/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */
#ifndef LCD_SCHEDULER_H
#define LCD_SCHEDULER_H

#include "../../mower/mower.hpp"

enum lcd_menu_scheduler
{
    SCHEDULER_START_M = 0,
    SCHEDULER_MON = 1,
    SCHEDULER_TUE = 2,
    SCHEDULER_WED = 3,
    SCHEDULER_THU = 4,
    SCHEDULER_FRI = 5,
    SCHEDULER_SAT = 6,
    SCHEDULER_SUN = 7
};
lcd_menu_scheduler _currentSchedulMenu;

void Lcd_navigation_menu_scheduler(mower_manager *mower)
{
    mower->lcd.process_sub_menu_input(_currentSchedulMenu, 6);
    mower->lcd.get_screen().setCursor(5, 0);

    // SHOW MENU
    switch (_currentSchedulMenu)
    {
    case SCHEDULER_START_M:
        mower->lcd.get_screen().print("> START");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().print(mower->schedul.is_time_to_mown() ? "yes" : "no");
        break;

    case SCHEDULER_MON:
        mower->lcd.get_screen().print("> MONDAY");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().print(mower->schedul.get_string_schedul_day(1));
        break;

    case SCHEDULER_TUE:
        mower->lcd.get_screen().print("> TUESDAY");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().print(mower->schedul.get_string_schedul_day(2));
        break;

    case SCHEDULER_WED:
        mower->lcd.get_screen().print("> WEDNESDAY");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().print(mower->schedul.get_string_schedul_day(3));
        break;

    case SCHEDULER_THU:
        mower->lcd.get_screen().print("> THURSDAY");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().print(mower->schedul.get_string_schedul_day(4));
        break;

    case SCHEDULER_FRI:
        mower->lcd.get_screen().print("> FRIDAY");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().print(mower->schedul.get_string_schedul_day(5));
        break;

    case SCHEDULER_SAT:
        mower->lcd.get_screen().print("> SATURDAY");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().print(mower->schedul.get_string_schedul_day(6));
        break;

    case SCHEDULER_SUN:
        mower->lcd.get_screen().print("> SUNDAY");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().print(mower->schedul.get_string_schedul_day(7));
        break;

    default:
        break;
    }
}

#endif