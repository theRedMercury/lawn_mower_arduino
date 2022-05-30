/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "wifi.hpp"
#include "../mower/mower.hpp"

#include "_personal_conf.h"
/*  File contains your personal wifi stuf like :

// WIFI Settings
const char *ssid = "";      // Enter your WiFi name
const char *password = "";  // Enter WiFi password
// MQTT Broker
const char *mqtt_port = "";
const char *mqtt_ip = "";   // Hostname not suported
const char *mqtt_user = "";
const char *mqtt_pass = "";

*/

/* MQTT Topic
 *
 * IN (from mower) -----------------------------------------
 * mower/in/ping        : "ping" to inform the server
 * mower/in/esp         : "ping" to inform the server (from wifi module)
 * mower/in/is_raining  : 0 / 1
 * mower/in/elec        : {volt, amp, char}
 * mower/in/wire        : {in, mag}
 * mower/in/sensor      : {temp, psi}
 * mower/in/status      : current mower status (RUNNING, ...)
 *
 * OUT (from external controller) --------------------------
 * mower/out/scheduler :
 *          "dayWeekNumber/startHour_EndHour" example : "1/8:00_12:00" -> Monday 8 to 12
 * mower/out/mode :
 *          "mode" example : "stop" / "run" / "station"
 *
 */

void wifi_control::setup()
{
    DEBUG_PRINT("SETUP : ");
    DEBUG_PRINT(class_name);

    SERIAL_WIFI.begin(SERIAL_BAUDRATE);

    _is_ready = true;
    // Init wifi settings
    unsigned char bit_result = 0;
    send_msg("ssid", ssid);
    bit_result |= 1UL << 0;
    delay(100);
    send_msg("pass", password);
    bit_result |= 1UL << 1;
    delay(200);
    send_msg("mqtt_ip", mqtt_ip);
    bit_result |= 1UL << 2;
    delay(100);
    send_msg("mqtt_port", mqtt_port);
    bit_result |= 1UL << 3;
    delay(100);
    send_msg("mqtt_user", mqtt_user);
    bit_result |= 1UL << 4;
    delay(200);
    send_msg("mqtt_pass", mqtt_pass);
    bit_result |= 1UL << 5;
    delay(500);
    _is_ready = false;
    _delay_update.reset_delay();
    DEBUG_PRINTLN(" : DONE > " + String(bit_result));
}

void wifi_control::update()
{
    // PING
    if (_delay_update.is_time_out())
    {
        send_msg("ping", "ping");

        send_msg("is_raining", String(mower->rain.is_rainning()).c_str());

        send_msg("elec", mower->elec.get_json().c_str());

        send_msg("wire", mower->perim.get_json().c_str());

        send_msg("sensor", mower->gyro.get_json().c_str());

        send_msg("status", mower->get_current_status_str());
        _delay_update.reset_delay();
    }

    // Update GPS
    if (mower->gps.is_updated() && mower->gps.is_lat_lon_valid())
    {
        send_msg("gps", mower->gps.get_json().c_str());
    }

    if (SERIAL_WIFI.available() > 12)
    {
        String msg = SERIAL_WIFI.readStringUntil('\n');
        msg.trim();
        String topic = _split(msg);
        String message = _split(msg, 1);
        topic.trim();

        DEBUG_PRINT("MQTT : ");
        DEBUG_PRINT(topic);
        DEBUG_PRINT(" > ");
        DEBUG_PRINTLN(message);

        for (unsigned short i = 0; i <= message.length(); i++)
        {
            if (message.charAt(i) < 0x20) // ASCII
            {
                message.setCharAt(i, ' ');
            }
        }
        message.trim();

        if (topic.startsWith("mower/"))
        {
            _last_msg = msg;

            DEBUG_PRINT("MQTT : ");
            DEBUG_PRINT(topic);
            DEBUG_PRINT(" > ");
            DEBUG_PRINTLN(message);

            if (topic == "mower/msg" && message == "Ready")
            {
                DEBUG_PRINTLN("WIFI is ok");
                _is_ready = true;
                send_msg("status", mower->get_current_status_str());
            }
            if (topic == "mower/out/scheduler") // msg : 1/8:00_12:00
            {
                _process_scheduler(message);
            }
            if (topic == "mower/out/mode") // msg : stop; go; station;
            {
                _process_mode(message);
                send_msg("status", mower->get_current_status_str());
            }
        }
    }
}

void wifi_control::send_msg(const char *topic, const char *msg)
{
    if (_is_ready)
    {
        String payload = String(topic) + String('>') + String(msg);
        payload.trim();
        payload += String('\n');
        DEBUG_PRINTLN("MQTT PUB > " + payload);
        SERIAL_WIFI.print(payload);
        delay(5);
    }
}

bool wifi_control::is_ready() const
{
    return _is_ready;
}

const char *wifi_control::get_last_msg() const
{
    return _last_msg.c_str();
}

void wifi_control::_process_mode(String message)
{
    message.trim();
    message = _clear_string(message);
    DEBUG_PRINTLN("_process_mode : " + String(message));
    if (message == "stop")
    {
        mower->set_current_status(mower_status::WAITING);
        return;
    }
    if (message == "run")
    {
        mower->schedul.force_mowing();
        mower->set_current_status(mower_status::RUNNING);
        return;
    }
    if (message == "station")
    {
        mower->set_current_status(mower_status::RETURN_STATION);
        return;
    }
}

void wifi_control::_process_scheduler(String message)
{
    String day_num = _split(message, 0, '/');
    if (!_is_integer(day_num, 1, 7))
    {
        return;
    }
    message = _split(message, 1, '/');

    String time_start = _split(message, 0, '_');
    String hs = _split(time_start, 0, ':');
    if (!_is_integer(hs, 1, 24))
    {
        return;
    }
    String ms = _split(time_start, 1, ':');
    if (!_is_integer(ms, 0, 60))
    {
        return;
    }

    String time_end = _split(message, 1, '_');
    String he = _split(time_end, 0, ':');
    if (!_is_integer(he, 1, 24))
    {
        return;
    }
    String me = _split(time_end, 1, ':');
    if (!_is_integer(me, 0, 60))
    {
        return;
    }

    unsigned char day_week_num, h_start, m_start, h_end, m_end;
    day_week_num = constrain(day_num.toInt(), 1, 7);
    h_start = constrain(hs.toInt(), 6, 22);
    m_start = constrain(ms.toInt(), 0, 59);
    h_end = constrain(he.toInt(), 6, 22);
    m_end = constrain(me.toInt(), 0, 59);
    mower->schedul.set_scheduler(day_week_num, h_start, m_start, h_end, m_end);

    DEBUG_PRINT("SCHEDULER   ");
    DEBUG_PRINT(day_num);
    DEBUG_PRINT("  >  ");
    DEBUG_PRINT(hs);
    DEBUG_PRINT(" : ");
    DEBUG_PRINT(ms);
    DEBUG_PRINT("  /  ");
    DEBUG_PRINT(he);
    DEBUG_PRINT(" : ");
    DEBUG_PRINTLN(me);
}

String wifi_control::_clear_string(String data)
{
    String ret = "";
    for (unsigned int i = 0; i <= data.length(); i++)
    {
        if (isalpha(data.charAt(i)) && data.charAt(i) != ' ')
        {
            ret += String(data.charAt(i));
        }
    }
    return ret;
}

bool wifi_control::_is_integer(String msg, int min, int max)
{
    for (unsigned int i = 0; i >= msg.length(); i++)
    {
        if (!isDigit(msg.charAt(i)))
        {
            return false;
        }
    }
    return msg.toInt() >= min && msg.toInt() <= max;
}

String wifi_control::_split(const String data, const unsigned int index, const char separator)
{
    unsigned int found = 0;
    int str_index[] = {0, -1};
    int max_index = data.length() - 1;
    for (int i = 0; i <= max_index && found <= index; i++)
    {
        if (data.charAt(i) == separator || i == max_index)
        {
            found++;
            str_index[0] = str_index[1] + 1;
            str_index[1] = (i == max_index) ? i + 1 : i;
        }
    }
    return found > index ? data.substring(str_index[0], str_index[1]) : "";
}