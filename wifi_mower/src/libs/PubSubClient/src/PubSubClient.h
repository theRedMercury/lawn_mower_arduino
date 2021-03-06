/*
 PubSubClient.h - A simple client for MQTT.
  Nick O'Leary
  http://knolleary.net
*/

#ifndef PubSubClient_h
#define PubSubClient_h

#include <Arduino.h>
#include "IPAddress.h"
#include "Client.h"
#include "Stream.h"

#define MQTT_VERSION_3_1 3
#define MQTT_VERSION_3_1_1 4

// MQTT_VERSION : Pick the version
//#define MQTT_VERSION MQTT_VERSION_3_1
#ifndef MQTT_VERSION
#define MQTT_VERSION MQTT_VERSION_3_1_1
#endif

// MQTT_MAX_PACKET_SIZE : Maximum packet size. Override with setBufferSize().
#ifndef MQTT_MAX_PACKET_SIZE
#define MQTT_MAX_PACKET_SIZE 256
#endif

// MQTT_KEEPALIVE : keepAlive interval in Seconds. Override with setKeepAlive()
#ifndef MQTT_KEEPALIVE
#define MQTT_KEEPALIVE 15
#endif

// MQTT_SOCKET_TIMEOUT: socket timeout interval in Seconds. Override with setSocketTimeout()
#ifndef MQTT_SOCKET_TIMEOUT
#define MQTT_SOCKET_TIMEOUT 15
#endif

// MQTT_MAX_TRANSFER_SIZE : limit how much data is passed to the network client
//  in each write call. Needed for the Arduino Wifi Shield. Leave undefined to
//  pass the entire MQTT packet in each write call.
//#define MQTT_MAX_TRANSFER_SIZE 80

// Possible values for client.state()
#define MQTT_CONNECTION_TIMEOUT -4
#define MQTT_CONNECTION_LOST -3
#define MQTT_CONNECT_FAILED -2
#define MQTT_DISCONNECTED -1
#define MQTT_CONNECTED 0
#define MQTT_CONNECT_BAD_PROTOCOL 1
#define MQTT_CONNECT_BAD_CLIENT_ID 2
#define MQTT_CONNECT_UNAVAILABLE 3
#define MQTT_CONNECT_BAD_CREDENTIALS 4
#define MQTT_CONNECT_UNAUTHORIZED 5

#define MQTTCONNECT 1 << 4      // Client request to connect to Server
#define MQTTCONNACK 2 << 4      // Connect Acknowledgment
#define MQTTPUBLISH 3 << 4      // Publish message
#define MQTTPUBACK 4 << 4       // Publish Acknowledgment
#define MQTTPUBREC 5 << 4       // Publish Received (assured delivery part 1)
#define MQTTPUBREL 6 << 4       // Publish Release (assured delivery part 2)
#define MQTTPUBCOMP 7 << 4      // Publish Complete (assured delivery part 3)
#define MQTTSUBSCRIBE 8 << 4    // Client Subscribe request
#define MQTTSUBACK 9 << 4       // Subscribe Acknowledgment
#define MQTTUNSUBSCRIBE 10 << 4 // Client Unsubscribe request
#define MQTTUNSUBACK 11 << 4    // Unsubscribe Acknowledgment
#define MQTTPINGREQ 12 << 4     // PING Request
#define MQTTPINGRESP 13 << 4    // PING Response
#define MQTTDISCONNECT 14 << 4  // Client is Disconnecting
#define MQTTReserved 15 << 4    // Reserved

#define MQTTQOS0 (0 << 1)
#define MQTTQOS1 (1 << 1)
#define MQTTQOS2 (2 << 1)

// Maximum size of fixed header and variable length size header
#define MQTT_MAX_HEADER_SIZE 5

#if defined(ESP8266) || defined(ESP32)
#include <functional>
#define MQTT_CALLBACK_SIGNATURE std::function<void(char *, unsigned char *, unsigned int)> callback
#else
#define MQTT_CALLBACK_SIGNATURE void (*callback)(char *, unsigned char *, unsigned int)
#endif

#define CHECK_STRING_LENGTH(l, s)                               \
   if (l + 2 + strnlen(s, this->bufferSize) > this->bufferSize) \
   {                                                            \
      _client->stop();                                          \
      return false;                                             \
   }

class PubSubClient : public Print
{
private:
   Client *_client;
   unsigned char *buffer;
   unsigned short bufferSize;
   unsigned short keepAlive;
   unsigned short socketTimeout;
   unsigned short nextMsgId;
   unsigned long lastOutActivity;
   unsigned long lastInActivity;
   bool pingOutstanding;
   MQTT_CALLBACK_SIGNATURE;
   unsigned int readPacket(unsigned char *);
   bool readByte(unsigned char *result);
   bool readByte(unsigned char *result, unsigned short *index);
   bool write(unsigned char header, unsigned char *buf, unsigned short length);
   unsigned short writeString(const char *string, unsigned char *buf, unsigned short pos);
   // Build up the header ready to send
   // Returns the size of the header
   // Note: the header is built at the end of the first MQTT_MAX_HEADER_SIZE bytes, so will start
   //       (MQTT_MAX_HEADER_SIZE - <returned size>) bytes into the buffer
   size_t buildHeader(unsigned char header, unsigned char *buf, unsigned short length);
   IPAddress ip;
   const char *domain;
   unsigned short port;
   Stream *stream;
   int _state;

public:
   PubSubClient();
   PubSubClient(Client &client);
   PubSubClient(IPAddress, unsigned short, Client &client);
   PubSubClient(IPAddress, unsigned short, Client &client, Stream &);
   PubSubClient(IPAddress, unsigned short, MQTT_CALLBACK_SIGNATURE, Client &client);
   PubSubClient(IPAddress, unsigned short, MQTT_CALLBACK_SIGNATURE, Client &client, Stream &);
   PubSubClient(unsigned char *, unsigned short, Client &client);
   PubSubClient(unsigned char *, unsigned short, Client &client, Stream &);
   PubSubClient(unsigned char *, unsigned short, MQTT_CALLBACK_SIGNATURE, Client &client);
   PubSubClient(unsigned char *, unsigned short, MQTT_CALLBACK_SIGNATURE, Client &client, Stream &);
   PubSubClient(const char *, unsigned short, Client &client);
   PubSubClient(const char *, unsigned short, Client &client, Stream &);
   PubSubClient(const char *, unsigned short, MQTT_CALLBACK_SIGNATURE, Client &client);
   PubSubClient(const char *, unsigned short, MQTT_CALLBACK_SIGNATURE, Client &client, Stream &);

   ~PubSubClient();

   PubSubClient &setServer(IPAddress ip, unsigned short port);
   PubSubClient &setServer(unsigned char *ip, unsigned short port);
   PubSubClient &setServer(const char *domain, unsigned short port);
   PubSubClient &setCallback(MQTT_CALLBACK_SIGNATURE);
   PubSubClient &setClient(Client &client);
   PubSubClient &setStream(Stream &stream);
   PubSubClient &setKeepAlive(unsigned short keepAlive);
   PubSubClient &setSocketTimeout(unsigned short timeout);

   bool setBufferSize(unsigned short size);
   unsigned short getBufferSize();

   bool connect(const char *id);
   bool connect(const char *id, const char *user, const char *pass);
   bool connect(const char *id, const char *willTopic, unsigned char willQos, bool willRetain, const char *willMessage);
   bool connect(const char *id, const char *user, const char *pass, const char *willTopic, unsigned char willQos, bool willRetain, const char *willMessage);
   bool connect(const char *id, const char *user, const char *pass, const char *willTopic, unsigned char willQos, bool willRetain, const char *willMessage, bool cleanSession);
   void disconnect();
   bool publish(const char *topic, const char *payload);
   bool publish(const char *topic, const char *payload, bool retained);
   bool publish(const char *topic, const unsigned char *payload, unsigned int plength);
   bool publish(const char *topic, const unsigned char *payload, unsigned int plength, bool retained);
   bool publish_P(const char *topic, const char *payload, bool retained);
   bool publish_P(const char *topic, const unsigned char *payload, unsigned int plength, bool retained);
   // Start to publish a message.
   // This API:
   //   beginPublish(...)
   //   one or more calls to write(...)
   //   endPublish()
   // Allows for arbitrarily large payloads to be sent without them having to be copied into
   // a new buffer and held in memory at one time
   // Returns 1 if the message was started successfully, 0 if there was an error
   bool beginPublish(const char *topic, unsigned int plength, bool retained);
   // Finish off this publish message (started with beginPublish)
   // Returns 1 if the packet was sent successfully, 0 if there was an error
   int endPublish();
   // Write a single unsigned char of payload (only to be used with beginPublish/endPublish)
   virtual size_t write(unsigned char);
   // Write size bytes from buffer into the payload (only to be used with beginPublish/endPublish)
   // Returns the number of bytes written
   virtual size_t write(const unsigned char *buffer, size_t size);
   bool subscribe(const char *topic);
   bool subscribe(const char *topic, unsigned char qos);
   bool unsubscribe(const char *topic);
   bool loop();
   bool connected();
   int state();
};

#endif
