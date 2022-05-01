/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */
#ifndef LCD_MODE_H
#define LCD_MODE_H

#include "../../mower/mower.hpp"

enum lcd_menu_mode
{
    MODE_STATUS = 0,
    MODE_RUN = 1,
    MODE_RETURN_STATION = 2,
    MODE_TEST_NAVI = 3,
    MODE_TEST_CIRCLE = 4
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
        mower->set_current_status(mower->get_current_status() == mower_status::TESTING_NAV ? mower_status::READY : mower->get_current_status());

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
                mower->set_current_status(mower_status::RUNNING);
                mower->lcd.return_main_info();
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
            }
        }
        break;

    case MODE_TEST_NAVI:
        mower->lcd.get_screen().print("> TEST NAV");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        if (mower->get_current_status() == mower_status::TESTING_NAV)
        {
            mower->lcd.get_screen().print("OK");
        }
        else
        {
            mower->lcd.get_screen().print("OFF");
        }
        mower->lcd.get_screen().setCursor(4, 1);
        mower->lcd.get_screen().print(mower->motor.get_speed_left());
        mower->lcd.get_screen().setCursor(10, 1);
        mower->lcd.get_screen().print(mower->motor.get_speed_right());
        if (mower->get_current_status() == mower_status::TESTING_NAV)
        {
            _counter_nav++;
            if (_counter_nav > 600)
            {
                mower->motor.stop();
                mower->set_current_status(mower_status::READY);
            }
        }
        if (mower->lcd.get_current_input() == current_flag_input::INPUT_OK)
        {
            if (mower->get_current_status() == mower_status::TESTING_NAV)
            {
                mower->motor.stop();
                mower->set_current_status(mower_status::READY);
            }
            else
            {
                mower->set_current_status(mower_status::TESTING_NAV);
                mower->nav.update_target_angle();
                _counter_nav = 0;
            }
        }
        break;

    case MODE_TEST_CIRCLE:
        mower->lcd.get_screen().print("> TEST CIRCLE");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        if (mower->get_current_status() == mower_status::TESTING_NAV)
        {
            mower->lcd.get_screen().print("OK");
        }
        else
        {
            mower->lcd.get_screen().print("OFF");
        }
        mower->lcd.get_screen().setCursor(4, 1);
        mower->lcd.get_screen().print(mower->motor.get_speed_left());
        mower->lcd.get_screen().setCursor(10, 1);
        mower->lcd.get_screen().print(mower->motor.get_speed_right());
        if (mower->get_current_status() == mower_status::TESTING_NAV_02)
        {
            _counter_nav++;
            if (_counter_nav > 600)
            {
                mower->motor.stop();
                mower->set_current_status(mower_status::READY);
            }
        }
        if (mower->lcd.get_current_input() == current_flag_input::INPUT_OK)
        {
            if (mower->get_current_status() == mower_status::TESTING_NAV_02)
            {
                mower->motor.stop();
                mower->set_current_status(mower_status::READY);
            }
            else
            {
                mower->set_current_status(mower_status::TESTING_NAV_02);
                mower->nav.update_target_angle();
                _counter_nav = 0;
            }
        }
        break;
    default:
        break;
    }
}

#endif