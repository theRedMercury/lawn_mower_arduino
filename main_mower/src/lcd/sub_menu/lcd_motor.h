/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */
#ifndef LCD_MOTOR_H
#define LCD_MOTOR_H

#include "../../mower/mower.hpp"

enum lcd_menu_motor : char
{
    MOTOR_TEST = 0,
    MOTOR_LEFT = 1,
    MOTOR_RIGHT = 2,
    MOTOR_BLADE = 3

};
lcd_menu_motor _currentMotorMenu;

bool _motor_test_on = false;
unsigned short _time_motor_test = 0;

void _test_motor_basic(mower_manager *mower);
void _test_motor_blade(mower_manager *mower);
void _test_motor_left(mower_manager *mower);
void _test_motor_right(mower_manager *mower);
void _print_motor_speed(mower_manager *mower);

void Lcd_navigation_menu_motor(mower_manager *mower)
{

    switch (mower->lcd.process_sub_menu_input(_currentMotorMenu, 3))
    {
    case current_flag_input::INPUT_LEFT:
        if (static_cast<int>(_currentMotorMenu) == 0)
        {
            _motor_test_on = false;
            _time_motor_test = 0;
        }

    default:
        break;
    }

    mower->lcd.get_screen().setCursor(5, 0);

    // Show Menu
    switch (_currentMotorMenu)
    {
    case MOTOR_TEST:
        mower->lcd.get_screen().print("> Test 01");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        if (mower->lcd.get_current_input() == current_flag_input::INPUT_OK && _time_motor_test == 0)
        {
            _motor_test_on = true;
        }
        if (mower->lcd.get_current_input() == current_flag_input::INPUT_OK && _time_motor_test > 2)
        {
            _motor_test_on = false;
        }
        _test_motor_basic(mower);
        break;

    case MOTOR_LEFT:
        mower->lcd.get_screen().print(">Test Left");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        if (mower->lcd.get_current_input() == current_flag_input::INPUT_OK && _time_motor_test == 0)
        {
            _motor_test_on = true;
        }
        if (mower->lcd.get_current_input() == current_flag_input::INPUT_OK && _time_motor_test > 2)
        {
            _motor_test_on = false;
        }
        _test_motor_left(mower);
        break;
    case MOTOR_RIGHT:
        mower->lcd.get_screen().print(">Test Right");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        if (mower->lcd.get_current_input() == current_flag_input::INPUT_OK && _time_motor_test == 0)
        {
            _motor_test_on = true;
        }
        if (mower->lcd.get_current_input() == current_flag_input::INPUT_OK && _time_motor_test > 2)
        {
            _motor_test_on = false;
        }
        _test_motor_right(mower);
        break;

    case MOTOR_BLADE:
        mower->lcd.get_screen().print("> Blade");
        mower->lcd.clear_line(1);
        mower->lcd.get_screen().setCursor(0, 1);
        if (mower->lcd.get_current_input() == current_flag_input::INPUT_OK && _time_motor_test == 0)
        {
            _motor_test_on = true;
        }
        if (mower->lcd.get_current_input() == current_flag_input::INPUT_OK && _time_motor_test > 2)
        {
            _motor_test_on = false;
        }
        _test_motor_blade(mower);
    default:
        break;
    }
}

void _test_motor_blade(mower_manager *mower)
{
    if (!_motor_test_on)
    {
        mower->lcd.get_screen().print("...");
        mower->motor_blade.stop();
        _time_motor_test = 0;
        return;
    }
    mower->lcd.get_screen().print("Progress ");
    mower->lcd.get_screen().print(mower->motor_blade.is_on());
    mower->lcd.get_screen().setCursor(9, 1);
    switch (_time_motor_test)
    {
    case 0:
        mower->motor_blade.stop();
        break;
    case 1:
        mower->motor_blade.on();
        break;
    case 120:
        mower->motor_blade.stop();
        _motor_test_on = false;
        break;
    default:
        break;
    }
    _time_motor_test++;
}

void _test_motor_left(mower_manager *mower)
{
    if (!_motor_test_on)
    {
        mower->lcd.get_screen().print("...");
        mower->motor.stop();
        _time_motor_test = 0;
        return;
    }
    switch (_time_motor_test)
    {
    case 0:
        mower->motor.set(0, 0);
        break;
    case 1:
        mower->motor.set(255, 0);
        break;
    case 20:
        mower->motor.set(0, 0);
    case 21:
        mower->motor.set(-255, 0);
    case 40:
        mower->motor.set(0, 0);
        _motor_test_on = false;
        break;
    default:
        break;
    }
    _print_motor_speed(mower);
    _time_motor_test++;
}
void _test_motor_right(mower_manager *mower)
{
    if (!_motor_test_on)
    {
        mower->lcd.get_screen().print("...");
        mower->motor.stop();
        _time_motor_test = 0;
        return;
    }
    switch (_time_motor_test)
    {
    case 0:
        mower->motor.set(0, 0);
        break;
    case 1:
        mower->motor.set(0, 255);
        break;
    case 20:
        mower->motor.set(0, 0);
    case 21:
        mower->motor.set(0, -255);
    case 40:
        mower->motor.set(0, 0);
        _motor_test_on = false;
        break;
    }
    _print_motor_speed(mower);
    _time_motor_test++;
}
void _test_motor_basic(mower_manager *mower)
{
    if (!_motor_test_on)
    {
        mower->lcd.get_screen().print("...");
        mower->motor.stop();
        _time_motor_test = 0;
        return;
    }

    switch (_time_motor_test)
    {
    case 0:
        mower->motor.set(255, 255);
        break;
    case 20:
        mower->motor.set(0, 0);
        break;
    case 30:
        mower->motor.set(-255, -255);
        break;
    case 50:
        mower->motor.set(0, 0);
        _motor_test_on = false;
        break;
    default:
        break;
    }
    _print_motor_speed(mower);
    _time_motor_test++;
}

void _print_motor_speed(mower_manager *mower)
{
    mower->lcd.get_screen().print(">>>");
    mower->lcd.get_screen().setCursor(4, 1);
    mower->lcd.get_screen().print(mower->motor.get_speed_left());
    mower->lcd.get_screen().setCursor(10, 1);
    mower->lcd.get_screen().print(mower->motor.get_speed_right());
}
#endif