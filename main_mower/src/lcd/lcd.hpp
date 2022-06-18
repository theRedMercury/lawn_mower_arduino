/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */
#ifndef LCD_H
#define LCD_H

#include "../libs/LiquidCrystal_I2C/LiquidCrystal_I2C.h"

#include "../mower/abs_mower.hpp"

#define PIN_BUTTON_OK 44
#define PIN_BUTTON_LEFT 46
#define PIN_BUTTON_RIGHT 42

#define MAX_IDLE_SCREEN 120
#define MAX_OFF_SCREEN 180
#define LCD_ADRESS 0x27
#define LCD_CLEAR_FIELD "                " // 16 empty
#define LCD_X 16
#define LCD_Y 2

enum class current_flag_input : char
{
    INPUT_NONE,
    INPUT_OK,
    INPUT_LEFT,
    INPUT_RIGHT
};

class lcd_control : public abs_mower_class
{
public:
    using abs_mower_class::abs_mower_class;

    void setup();
    void update();
    void on_backlight();
    void off_backlight();
    void clear();
    void clear_line(const unsigned char line = 0);

    void show_message(const char *line1, const char *line2);
    void show_message(int posX, int posY, const char *line);
    void show_main_info();

    void return_main_info();
    void exit_sub_menu();

    LiquidCrystal_I2C get_screen() const;
    current_flag_input get_current_input() const;
    template <typename T>
    current_flag_input process_sub_menu_input(T &current_menu, unsigned char max);

private:
    void _process_input();

    enum class lcd_menu : char
    {
        STATUS = 0,
        MODE = 1,
        WIRE = 2,
        SENSOR = 3,
        GPS = 4,
        MOTOR = 5,
        WIFI = 6,
        TEST = 7,
        SCHEDUL = 8,
        DEBUG = 9
    };

    current_flag_input _current_input = current_flag_input::INPUT_NONE;
    current_flag_input _last_input = current_flag_input::INPUT_NONE;
    LiquidCrystal_I2C _lcd_I2C{LCD_ADRESS, LCD_X, LCD_Y};
    lcd_menu _current_main_menu = lcd_menu::STATUS;
    bool _screen_is_off = false;
    bool _in_sub_menu = false;
    unsigned short _counter_loop_idle = 0;
};

#endif