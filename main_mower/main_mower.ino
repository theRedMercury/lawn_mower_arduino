/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 *  
 *  @target Arduino mega
 */

//////////////////////////////////////////////////
// TIMER SOFT
//////////////////////////////////////////////////
#define USE_TIMER_1 false
#define USE_TIMER_2 false
#define USE_TIMER_3 false
#define USE_TIMER_4 false
#define USE_TIMER_5 true

//#define SERIAL_RX_BUFFER_SIZE 256
#include "src/mower/mower.hpp"
static mower_manager mower;

#ifdef DEBUG_SPEED_MODE
unsigned long main_loop_millis;
unsigned long frame_counter;
#endif

///////////////////////////////////////////////////////
// TimerInterrupt
///////////////////////////////////////////////////////
#include "src/libs/timer_interrupt/TimerInterrupt.h"
#define TIMER_INTERVAL_MS 100000UL
volatile uint8_t _timer_reboot_wathdog = 0;
// Restart Arduino
void (*restart_arduino_func)(void) = 0;

void timer_watch_dog_handler()
{
  _timer_reboot_wathdog++;
  if (_timer_reboot_wathdog > 10)
  {
    mower.set_error();
    DEBUG_PRINTLN(">>>> TimerWatchdogHandler <<<<");
    delay(1000);
    restart_arduino_func();
  }
}

//////////////////////////////////////////////////////////////////////////////////
// SETUP
//////////////////////////////////////////////////////////////////////////////////
void setup()
{
#ifdef DEBUG_PRINTER
  DEBUG_PRINTER.begin(115200);
#endif
#ifdef DEBUG_SPEED_MODE
  main_loop_millis = 0;
  frame_counter = 0;
#endif

  DEBUG_PRINTLN("##########");
  DEBUG_PRINTLN("#  INIT  #");
  DEBUG_PRINTLN("##########");

  pinMode(LED_BUILTIN, OUTPUT);
  mower.setup();

  // Watchdog Safety //////////////////////////////////////////////////////////////
  if (!ITimer5.attachInterruptInterval(TIMER_INTERVAL_MS, timer_watch_dog_handler))
  {
    // Fail attach interrup : normally, never happen
    mower.set_error();
    mower.lcd.show_message("> FATAL ERROR   ", "Watchdog Timer !");
    DEBUG_PRINTLN("ERROR Watchdog timer...");
  }
  ////////////////////////////////////////////////////////////////////////////////
}

//////////////////////////////////////////////////////////////////////////////////
// MAIN LOOP
//////////////////////////////////////////////////////////////////////////////////
void loop()
{
  // Mower main process loop =======================
  mower.update();
  //================================================

  // DEBUG PRINT STAT LOOP =========================
#ifdef DEBUG_SPEED_MODE
  DEBUG_PRINTER.print(">>>>>   LOOP TIME : ");
  DEBUG_PRINTER.print((millis() - main_loop_millis));
  DEBUG_PRINTER.print(" (ms) - FRAME : ");
  DEBUG_PRINTER.print(frame_counter);
  DEBUG_PRINTER.println(" <<<<<\n\n");
  main_loop_millis = millis();
  frame_counter++;
#endif
  //================================================

  // Watchdog counter
  _timer_reboot_wathdog = 0;
}
//////////////////////////////////////////////////////////////////////////////////
