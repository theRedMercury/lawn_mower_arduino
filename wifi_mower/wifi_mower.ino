/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 *
 *  @target ESP8266 Generic module
 */

#define SERIAL_BUFFER_SIZE 128

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "src/libs/PubSubClient/src/PubSubClient.h"

/////////////////////////////////////////////////////////////
/// ESP8266 3.0.2 : Generic Module
/// WHEN UPLOAD : PUSH MODE BUTTON ON ALWAYS DURING UPLOAD //
///
/// Arduino MEGA 2560 With WiFi Built-in - ESP8266
/// UPLOAD  : 1 - 4 : OFF
///           5 - 7 : ON
///////////////////////////
/// RUN     : 1 - 4 : ON
///           5 - 7 : OFF
/////////////////////////////////////////////////////////////

#define SERIAL_WIFI Serial
#define SERIAL_BAUDRATE 9600

// WiFi
String ssid, password;
// MQTT Broker
String mqtt_ip, mqtt_user, mqtt_pass, mqtt_port, mqtt_client_id;

const char *topic_all = "mower/out/#";
const char *topic_ping = "mower/in/esp";
const char *topic_ping_msg = "ping";
const char *topic_msg = "mower/msg";

WiFiClient esp_client;
PubSubClient mqtt_client(esp_client);
unsigned int _counter_ping;

void inline print_topic(const char *topic, const char *msg)
{
    String payload = String(topic) + String('>') + String(msg);
    payload.trim();
    payload += String('\n');
    while (SERIAL_WIFI.availableForWrite() < payload.length())
    {
    }
    SERIAL_WIFI.print(payload);
}

void topic_callback(char *topic, unsigned char *payload, unsigned int length)
{
    char buf[length];
    memset(buf, 0, length);
    memcpy(buf, payload, length);
    String data(buf);
    data = data.substring(0, length);
    data.trim();
    print_topic(topic, data.c_str());
}

void init_wifi()
{
    WiFi.mode(WIFI_STA);
    // connecting to a WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        print_topic(topic_msg, "Connecting");
        delay(1000);
    }
    print_topic(topic_msg, "Connected");
    delay(200);
}

void init_mqtt()
{
    while (!mqtt_client.connected())
    {
        if (!mqtt_client.connect(mqtt_client_id.c_str(), mqtt_user.c_str(), mqtt_pass.c_str()))
        {
            print_topic(topic_msg, String(String("Error : ") + String(mqtt_client.state())).c_str());
            delay(2000);
        }
    }

    if (mqtt_client.subscribe(topic_all) && mqtt_client.publish(topic_msg, "Ready"))
    {
        print_topic(topic_msg, "Ready"); // Inform Mega status OK
    }
    else
    {
        print_topic(topic_msg, "Error");
    }
}

String inline _split(const String data, const unsigned int index = 0, const char separator = '>')
{
    unsigned int found = 0;
    int str_index[] = {0, -1};
    int max_index = data.length() - 1;
    String ret = "";
    for (int i = 0; i <= max_index && found <= index; i++)
    {
        if (data.charAt(i) == separator || i == max_index)
        {
            found++;
            str_index[0] = str_index[1] + 1;
            str_index[1] = (i == max_index) ? i + 1 : i;
        }
    }
    if (found > index)
    {
        ret = data.substring(str_index[0], str_index[1]);
        ret.trim();
    }
    return ret;
}

void setup()
{
    _counter_ping = 0;
    SERIAL_WIFI.begin(SERIAL_BAUDRATE);

    while (!SERIAL_WIFI.available())
    {
        delay(10);
    }

    unsigned char bit_result = 0;

    while (bit_result != 63) // = 0011 1111
    {
        String msg = SERIAL_WIFI.readStringUntil('\n');
        msg.trim();
        String topic = _split(msg);
        String message = _split(msg, 1);

        if (topic == "ssid")
        {
            ssid = message;
            bit_result |= 1UL << 0;
        }

        if (topic == "pass")
        {
            password = message;
            bit_result |= 1UL << 1;
        }

        if (topic == "mqtt_ip")
        {
            mqtt_ip = message;
            bit_result |= 1UL << 2;
        }

        if (topic == "mqtt_port")
        {
            mqtt_port = message;
            bit_result |= 1UL << 3;
        }

        if (topic == "mqtt_user")
        {
            mqtt_user = message;
            bit_result |= 1UL << 4;
        }

        if (topic == "mqtt_pass")
        {
            mqtt_pass = message;
            bit_result |= 1UL << 5;
        }
    }
    SERIAL_WIFI.flush();

    /* if (bit_result != 63) // = 0011 1111 -> All succes
     {
         String err = "INIT ERROR " + String(bit_result);
         print_topic(topic_msg, err.c_str());
     }*/

    // setup mqtt broker
    mqtt_client_id = "Mower-" + String(WiFi.macAddress());
    mqtt_client.setServer(mqtt_ip.c_str(), mqtt_port.toInt());
    mqtt_client.setCallback(topic_callback);

    init_wifi();
    init_mqtt();
}

void loop()
{
    _counter_ping++;
    if (mqtt_client.loop())
    {
        if (SERIAL_WIFI.available() >= 6)
        {
            String msg_in = SERIAL_WIFI.readStringUntil('\n');
            msg_in.trim();
            if (msg_in.length() > 0)
            {
                String topic_end = _split(msg_in);
                if (topic_end.length() > 1)
                {
                    String topic = String("mower/in/") + topic_end;

                    if (!mqtt_client.publish(topic.c_str(), _split(msg_in, 1).c_str()))
                    {
                        print_topic(topic_msg, String(String("Error pub : ") + String(mqtt_client.state())).c_str());
                    }
                }
            }
        }

        if (_counter_ping > 1000000)
        {
            _counter_ping = 0;
            if (!mqtt_client.publish(topic_ping, topic_ping_msg))
            {
                print_topic(topic_msg, String(String("Error ping : ") + String(mqtt_client.state())).c_str());
            }
        }
    }
    else
    {
        print_topic(topic_msg, String(String("Error : ") + String(mqtt_client.state())).c_str());
        init_mqtt();
    }

    if (!WiFi.isConnected())
    {
        print_topic(topic_msg, "WIFI lost");
        delay(1000 * 10);
        init_wifi();
        init_mqtt();
    }
}
