/* =============================================================
 * Project:   impact_driver_controller
 * File:      impact_driver_controller.ino
 * Author:    Martin Kriz
 * Company:   Ullmanna s.r.o.
 * Created:   2025-09-26
 * -------------------------------------------------------------
 * Description:
 *
 * Notes:
 * ============================================================= */

#include "Wire.h"
#include "include/Control_panel_display.h"
#include "include/Solenoid_mudule.h"
#include "include/ADS1115.h"

// State machine
enum class State {
  WAITING_FOR_START,
  DELAY,
  ACTIVATE_KNIFES,
  DELAY_CLOSING,
};
State currentState = State::WAITING_FOR_START;

enum class Modes {
  MANUAL,
  RANDOM,
  SKIPPING,
};
Modes currentMode = Modes::MANUAL;

constexpr long unsigned int  DELAY_MS = 10; // [ms] speed of main loop
constexpr unsigned int MAX_DELAY_MS   = 500; // [ms] maximum delay for knifes activation

ControlPanel panel;
SolenoidModule solenoid;
ADS1115Module adc;

bool error = false;
int knifeDelayMs = 0;

// Timer
unsigned long startTime = 0;
unsigned long delayTime = knifeDelayMs;
volatile bool alarm_fired = false;


int calculateKnifeDelayMs(int OldDelayMs)
{
  int offset = 1;
  int tmp = (1023 -panel.readPot1()) * MAX_DELAY_MS / 1023;
  if (tmp < (OldDelayMs - offset) || tmp > (OldDelayMs + offset))
  {
    //panel.showDelay(tmp);
    return tmp;
  }
  //panel.showDelay(OldDelayMs);
  return OldDelayMs;
}

int64_t alarm_callback(alarm_id_t id, void *user_data)
{
  alarm_fired = true;

  if (currentState == State::DELAY)
  {
    solenoid.KnifesClose();
    panel.setLedStatus(true);    
    currentState = State::ACTIVATE_KNIFES;
  }

  if (currentState == State::DELAY_CLOSING)
  {
    solenoid.KnifesOpen(); 
    panel.setLedStatus(false);   
    currentState = State::WAITING_FOR_START;
  }

  return 0;
}

void startTimerMs(uint32_t ms)
{
  if (ms == 0)
  {
    if (currentState == State::DELAY)
    {
      solenoid.KnifesClose();
      panel.setLedStatus(true);    
      currentState = State::ACTIVATE_KNIFES;
    }
    else if (currentState == State::DELAY_CLOSING)
    {
      solenoid.KnifesOpen(); 
      panel.setLedStatus(false);   
      currentState = State::WAITING_FOR_START;
    }
    return;
  }
  add_alarm_in_ms(ms, alarm_callback, NULL, false);
}



void setup() {
  //.begin(115200);
  //delay(1000);
  //Serial.println(F("Solenoid Testbench Starting..."));

  adc.begin();

  panel.beginDisplay();
  panel.showMode("Manual");
}

void loop() {
  panel.setLedError(error);

  currentMode = Modes::MANUAL;
  /*
  if (panel.wasSw1Pressed())
  {
    if (currentMode == Modes::MANUAL)
    {
      currentMode = Modes::RANDOM;
      panel.showMode("Random");
    }
    else if (currentMode == Modes::RANDOM)
    {
      currentMode = Modes::SKIPPING;
      panel.showMode("Skipping");
    }
    else
    {
      currentMode = Modes::MANUAL;
      panel.showMode("Manual");
    }
  }
  */

  knifeDelayMs = calculateKnifeDelayMs(knifeDelayMs);

  float solenoid_current = solenoid.readCurrent();
  //panel.showCurrent(solenoid_current);
  //Serial.print("Current: "); Serial.print(solenoid_current); Serial.println(" A");

  switch (currentState) {
    case State::WAITING_FOR_START:
      // Wait for start condition
      if (solenoid_current > 0.13)
      {
        delayTime = knifeDelayMs;
        solenoid.KnifesBeetween();
        currentState = State::DELAY;
        startTimerMs(delayTime);
      }
      break;

    case State::DELAY:
      break;

    case State::ACTIVATE_KNIFES:
      if (solenoid_current < 0.13) // Az odpadne signal knifes enable
      {
        delayTime = knifeDelayMs;
        solenoid.KnifesBeetween();
        currentState = State::DELAY_CLOSING;
        startTimerMs(delayTime);
      }
      break;

    case State::DELAY_CLOSING:
      break;    
  }

}