/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */
#ifndef LCD_MODE_H
#define LCD_MODE_H

#include "../../mower/mower.hpp"

enum lcd_menu_mode : char
{
    MODE_STATUS = 0,
    MODE_RUN = 1,
    MODE_RETURN_STATION = 2,
};
lcd_menu_mode _currentModeMenu;

int _counter_nav = 0;
void Lcd_navigation_menu_mode(mower_manager *mower)
{
    switch (mower->lcd.process_sub_menu_input(_currentModeMenu, 4))
    {
    case current_flag_input::INPUT_RIGHT:
    case current_flag_input::INPUT_LEFT:
        mower->motor.stop();
        mower->set_current_status(mower->get_current_status());

        break;
    default:
        break;
    }

    mower->lcd.get_screen().setCursor(5, 0);

    switch (_currentModeMenu)
    {
    case MODE_STATUS:
        mower->lcd.get_screen().print("> CURRENT");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        mower->lcd.get_screen().print(mower->get_current_status_str());
        break;

    case MODE_RUN:
        mower->lcd.get_screen().print("> RUN");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        if (mower->get_current_status() == mower_status::RUNNING)
        {
            mower->lcd.get_screen().print("OK");
        }
        else
        {
            mower->lcd.get_screen().print("OFF");
        }

        if (mower->lcd.get_current_input() == current_flag_input::INPUT_OK)
        {
            if (mower->get_current_status() == mower_status::RUNNING)
            {
                mower->set_current_status(mower_status::WAITING);
            }
            else
            {
                mower->schedul.force_mowing();
                mower->nav.start_mowing();
                mower->set_current_status(mower_status::RUNNING);
                if (mower->perim.get_simulate_is_inside())
                {
                    mower->lcd.return_debug_info();
                }
                else
                {
                    mower->lcd.return_debug_info();
                    // mower->lcd.return_main_info();
                }
            }
        }
        break;

    case MODE_RETURN_STATION:
        mower->lcd.get_screen().print("> STATION");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        if (mower->get_current_status() == mower_status::RETURN_STATION)
        {
            mower->lcd.get_screen().print("OK");
        }
        else
        {
            mower->lcd.get_screen().print("OFF");
        }

        if (mower->lcd.get_current_input() == current_flag_input::INPUT_OK)
        {
            if (mower->get_current_status() == mower_status::RETURN_STATION)
            {
                mower->set_current_status(mower_status::WAITING);
            }
            else
            {
                mower->set_current_status(mower_status::RETURN_STATION);
                mower->lcd.return_debug_info();
            }
        }
        break;
    default:
        break;
    }
}

#endif