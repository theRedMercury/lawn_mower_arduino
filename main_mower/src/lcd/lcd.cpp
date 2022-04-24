/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "lcd.hpp"
#include "../mower/mower.hpp"

#include "sub_menu/lcd_gps.h"
#include "sub_menu/lcd_mode.h"
#include "sub_menu/lcd_motor.h"
#include "sub_menu/lcd_scheduler.h"
#include "sub_menu/lcd_sensor.h"
#include "sub_menu/lcd_wifi.h"
#include "sub_menu/lcd_wire.h"

void lcd_control::setup()
{
    DEBUG_PRINT("SETUP : ");
    DEBUG_PRINT(class_name);

    _last_input = current_flag_input::INPUT_NONE;
    _current_main_menu = lcd_menu::STATUS;
    _counter_loop_idle = 0;
    _screen_is_off = false;
    _in_sub_menu = false;

    _lcd_I2C.init();
    _lcd_I2C.clear();
    on_backlight();
    show_message("> INIT ROBOT", NULL);

    pinMode(PIN_BUTTON_OK, INPUT);
    digitalWrite(PIN_BUTTON_OK, HIGH);

    pinMode(PIN_BUTTON_LEFT, INPUT);
    digitalWrite(PIN_BUTTON_LEFT, HIGH);

    pinMode(PIN_BUTTON_RIGHT, INPUT);
    digitalWrite(PIN_BUTTON_RIGHT, HIGH);

    DEBUG_PRINTLN(" : DONE");
}

void lcd_control::update()
{
    // Screen idle sleep
    if (_counter_loop_idle == MAX_IDLE_SCREEN)
        off_backlight();
    if (_counter_loop_idle == MAX_OFF_SCREEN)
        clear();
    if (_counter_loop_idle <= MAX_OFF_SCREEN + 1)
        _counter_loop_idle++;

    _process_input();

    if (_counter_loop_idle > MAX_OFF_SCREEN)
        return;

    // NAVIGATE INPUT
    if (!_in_sub_menu)
    {
        switch (_current_input)
        {
        case current_flag_input::INPUT_RIGHT:
            clear();
            if (static_cast<int>(_current_main_menu) < 8)
            {
                _current_main_menu = static_cast<lcd_menu>(static_cast<int>(_current_main_menu) + 1);
            }
            break;

        case current_flag_input::INPUT_LEFT:
            clear();
            if (static_cast<int>(_current_main_menu) > 0)
            {
                _current_main_menu = static_cast<lcd_menu>(static_cast<int>(_current_main_menu) - 1);
            }
            break;

        case current_flag_input::INPUT_OK:
            if (_current_main_menu != lcd_menu::STATUS)
            {
                _in_sub_menu = true;
                _current_input = current_flag_input::INPUT_NONE;
            }

            break;

        default:
            break;
        }
    }

    // SHOW MENU
    switch (_current_main_menu)
    {
    case lcd_menu::STATUS:

        show_message(mower->time.get_current_day(), mower->get_current_status_str());
        show_main_info();
        break;

    case lcd_menu::MODE:
        if (_in_sub_menu)
        {
            show_message(">MODE", NULL);
            Lcd_navigation_menu_mode(mower);
        }
        else
        {
            show_message("> MODE         ", LCD_CLEAR_FIELD);
        }
        break;

    case lcd_menu::SENSOR:
        if (_in_sub_menu)
        {
            show_message(">SENSOR", "");
            Lcd_navigation_menu_sensor(mower);
        }
        else
        {
            show_message("> SENSOR        ", LCD_CLEAR_FIELD);
        }
        break;

    case lcd_menu::GPS:
        if (_in_sub_menu)
        {
            show_message(">GPS ", "");
            Lcd_navigation_menu_gps(mower);
        }
        else
        {
            show_message("> GPS           ", LCD_CLEAR_FIELD);
        }
        break;

    case lcd_menu::WIRE:
        if (_in_sub_menu)
        {
            show_message(">WIRE ", "");
            Lcd_navigation_menu_wire(mower);
        }
        else
        {
            show_message("> WIRE          ", LCD_CLEAR_FIELD);
        }
        break;

    case lcd_menu::MOTOR:
        if (_in_sub_menu)
        {
            show_message(">MOTOR ", "");
            Lcd_navigation_menu_motor(mower);
        }
        else
        {
            show_message("> MOTOR         ", LCD_CLEAR_FIELD);
        }
        break;

    case lcd_menu::WIFI:
        if (_in_sub_menu)
        {
            show_message(">WIFI ", "");
            Lcd_navigation_menu_wifi(mower);
        }
        else
        {
            show_message("> WIFI         ", LCD_CLEAR_FIELD);
        }
        break;

    case lcd_menu::TEST:
        show_message("> TEST", LCD_CLEAR_FIELD);
        break;

    case lcd_menu::SCHEDUL:
        if (_in_sub_menu)
        {
            show_message(">SCHEDUL ", "");
            Lcd_navigation_menu_scheduler(mower);
        }
        else
        {
            show_message("> SCHEDUL         ", LCD_CLEAR_FIELD);
        }
        break;

    default:
        _current_main_menu = lcd_menu::STATUS;
        break;
    }
}

void lcd_control::on_backlight()
{
    _counter_loop_idle = 0;
    _screen_is_off = false;
    _lcd_I2C.setBacklight(HIGH);
}

void lcd_control::off_backlight()
{
    _screen_is_off = true;
    _lcd_I2C.setBacklight(LOW);
}

void lcd_control::clear()
{
    _lcd_I2C.clear();
}

void lcd_control::clear_line(const uint8_t line)
{
    _lcd_I2C.setCursor(0, line);
    for (uint8_t x = 0; x < 16; x++)
    {
        _lcd_I2C.print(' ');
    }
    _lcd_I2C.setCursor(0, line);
}

void lcd_control::show_message(const char *line1, const char *line2)
{
    // max 16
    if (line1 != NULL)
    {
        _lcd_I2C.setCursor(0, 0);
        _lcd_I2C.print(line1);
    }
    if (line2 != NULL)
    {
        _lcd_I2C.setCursor(0, 1);
        _lcd_I2C.print(line2);
    }
}

void lcd_control::show_message(int posX, int posY, const char *line)
{
    _lcd_I2C.setCursor(posX, posY);
    _lcd_I2C.print(line);
}

void lcd_control::show_main_info()
{
    // Current wire status
    _lcd_I2C.setCursor(7, 0);
    _lcd_I2C.print(mower->perim.is_inside() ? "IN" : "OUT");

    // Current Hour
    _lcd_I2C.setCursor(11, 0);
    if (mower->time.get_hour() < 10)
    {
        _lcd_I2C.print('0');
    }
    _lcd_I2C.print(String(mower->time.get_hour()));
    _lcd_I2C.print(':');
    if (mower->time.get_minute() < 10)
    {
        _lcd_I2C.print('0');
    }
    _lcd_I2C.print(String(mower->time.get_minute()));

    // Collision
    _lcd_I2C.setCursor(7, 1);
    _lcd_I2C.print(mower->nav.get_target());

    // Volt robot
    _lcd_I2C.setCursor(11, 1);
    _lcd_I2C.print(String(mower->elec.get_curent_volt(), 1));
    _lcd_I2C.setCursor(15, 1);
    _lcd_I2C.print('v');
}

void lcd_control::return_main_info()
{
    _in_sub_menu = false;
    clear();
    _current_main_menu = lcd_menu::STATUS;
}

void lcd_control::exit_sub_menu()
{
    _in_sub_menu = false;
}

LiquidCrystal_I2C lcd_control::get_screen() const
{
    return _lcd_I2C;
}

const current_flag_input lcd_control::get_current_input() const
{
    return _current_input;
}

template <typename T>
const current_flag_input lcd_control::process_sub_menu_input(T &current_menu, uint8_t max)
{
    on_backlight();
    switch (get_current_input())
    {
    case current_flag_input::INPUT_RIGHT:
        clear();
        if (current_menu < max)
        {
            current_menu = static_cast<T>(static_cast<int>(current_menu) + 1);
        }
        return current_flag_input::INPUT_RIGHT;
    case current_flag_input::INPUT_LEFT:
        clear();
        if (current_menu == 0)
        {
            exit_sub_menu();
        }
        if (current_menu > 0)
        {
            current_menu = static_cast<T>(static_cast<int>(current_menu) - 1);
        }
        return current_flag_input::INPUT_LEFT;
    default:
        break;
    }
    return current_flag_input::INPUT_NONE;
}

void lcd_control::_process_input()
{
    int _ok_status = digitalRead(PIN_BUTTON_OK);
    int _left_status = digitalRead(PIN_BUTTON_LEFT);
    int _right_status = digitalRead(PIN_BUTTON_RIGHT);

    // CLICK
    if (_ok_status == LOW && _last_input == current_flag_input::INPUT_NONE)
    {
        _current_input = (_screen_is_off) ? current_flag_input::INPUT_NONE : current_flag_input::INPUT_OK;
        if (_screen_is_off)
            on_backlight();
        _last_input = current_flag_input::INPUT_OK;
        _counter_loop_idle = 0;
        return;
    }
    if (_left_status == LOW && _last_input == current_flag_input::INPUT_NONE)
    {
        _current_input = (_screen_is_off) ? current_flag_input::INPUT_NONE : current_flag_input::INPUT_LEFT;
        if (_screen_is_off)
            on_backlight();
        _last_input = current_flag_input::INPUT_LEFT;
        _counter_loop_idle = 0;
        return;
    }
    if (_right_status == LOW && _last_input == current_flag_input::INPUT_NONE)
    {
        _current_input = (_screen_is_off) ? current_flag_input::INPUT_NONE : current_flag_input::INPUT_RIGHT;
        if (_screen_is_off)
            on_backlight();
        _last_input = current_flag_input::INPUT_RIGHT;
        _counter_loop_idle = 0;
        return;
    }
    // RELEASE
    if (_ok_status == HIGH && _last_input == current_flag_input::INPUT_OK)
        _last_input = current_flag_input::INPUT_NONE;
    if (_left_status == HIGH && _last_input == current_flag_input::INPUT_LEFT)
        _last_input = current_flag_input::INPUT_NONE;
    if (_right_status == HIGH && _last_input == current_flag_input::INPUT_RIGHT)
        _last_input = current_flag_input::INPUT_NONE;

    _current_input = current_flag_input::INPUT_NONE;
}
