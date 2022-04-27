/**
    @brief  lawn mower home
    @author Nicolas Masson <https://github.com/theRedMercury>
    @date   2021 - 2022

    @target Arduino nano
*/
#include "src/libs/TimerOne/TimerOne.h"

#define PIN_PERIM_01 2
#define PIN_PERIM_02 4
#define PIN_RELAY 7
#define PIN_FAN 8

#define BUMPER_1 10
#define BUMPER_2 11

#define SENDER_ARRAY_SIZE 24
#define MAX_WIRE_STILL_ON 50
#define MAX_RELAY_STILL_ON 5

volatile int index = 0;
volatile bool enableWire = true;
volatile const char signalCode[] = {1, 1, -1, -1, 1, -1, 1, -1, -1, 1, -1, 1, 1, -1, -1, 1, -1, -1, 1, -1, -1, 1, 1, -1};
bool mower_detected = false;

unsigned long time_wire_still_on = 0;
unsigned long time_relay_still_on = 0;

void timerCallback();

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_PERIM_01, OUTPUT);
  pinMode(PIN_PERIM_02, OUTPUT);
  pinMode(PIN_FAN, OUTPUT);

  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, HIGH);

  pinMode(BUMPER_1, INPUT);
  digitalWrite(BUMPER_1, HIGH);
  pinMode(BUMPER_2, INPUT);
  digitalWrite(BUMPER_2, HIGH);

  unsigned long timer = round(1000000.0 / 9615.0);
  Timer1.initialize(timer); // 9615 Hz
  Timer1.attachInterrupt(timerCallback);
}

void loop()
{
  int bumper_1 = digitalRead(BUMPER_1);
  int bumper_2 = digitalRead(BUMPER_2);

  mower_detected = (bumper_1 == LOW || bumper_2 == LOW);

  // mower incomming
  if (mower_detected)
  {
    time_relay_still_on = 0;
    digitalWrite(PIN_RELAY, LOW);
  }
  if (mower_detected && enableWire)
  {
    time_wire_still_on++;
  }
  if (mower_detected && time_wire_still_on > MAX_WIRE_STILL_ON)
  {
    time_wire_still_on = 0;
    enableWire = false;
  }

  // mower leave
  if (!mower_detected)
  {
    time_wire_still_on = 0;
    enableWire = true;
  }
  if (!mower_detected && enableWire)
  {
    time_relay_still_on++;
  }
  if (!mower_detected && time_relay_still_on > MAX_RELAY_STILL_ON)
  {
    time_relay_still_on = 0;
    digitalWrite(PIN_RELAY, HIGH);
  }

  if (enableWire)
  {
    digitalWrite(PIN_FAN, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
  else
  {
    digitalWrite(PIN_FAN, LOW);
  }

  // digitalWrite(PIN_RELAY, LOW);
  delay(100);
}

void timerCallback()
{
  if (enableWire)
  {
    switch (signalCode[index])
    {
    case 1:
      digitalWrite(PIN_PERIM_01, LOW);
      digitalWrite(PIN_PERIM_02, HIGH);
      break;
    case -1:
      digitalWrite(PIN_PERIM_01, HIGH);
      digitalWrite(PIN_PERIM_02, LOW);
      break;
    case 0:
      digitalWrite(PIN_PERIM_01, LOW);
      digitalWrite(PIN_PERIM_02, LOW);
      break;
    default:
      abort();
      break;
    }
    index++;
    if (index == SENDER_ARRAY_SIZE)
    {
      index = 0;
    }
  }
  else
  {
    index = 0;
    digitalWrite(PIN_PERIM_01, LOW);
    digitalWrite(PIN_PERIM_02, LOW);
  }
}
