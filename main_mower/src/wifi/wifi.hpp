/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#ifndef WIFI_H
#define WIFI_H
#include "../mower/abs_mower.hpp"

#define SERIAL_WIFI Serial3
#define COUNTER_MAX_MQTT_REFRESH 20000UL // every ~20 sec

class wifi_control : public abs_mower_class
{
public:
    using abs_mower_class::abs_mower_class;
    void setup();
    void update();
    void send_msg(const char *topic, const char *msg);

    const bool is_ready() const;
    const char *get_last_msg() const;

private:
    void _process_mode(String message);
    void _process_scheduler(String message);
    String _clear_string(String data);
    bool _is_integer(String msg, int min, int max);
    String _split(const String data, const uint32_t index = 0, const char separator = '>');

    bool _is_ready = false;
    unsigned long _counter_ping = 0;
    String _last_msg = "";
};
#endif
