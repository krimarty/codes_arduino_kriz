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

// State machine
enum class State {
  WAITING_FOR_START,
  DELAY,
  ACTIVATE_KNIFES,
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

bool error = false;
int knifeDelayMs = 0;

// Timer
unsigned long startTime = 0;
unsigned long delayTime = knifeDelayMs;

int calculateKnifeDelayMs(int OldDelayMs)
{
  int offset = 1;
  int tmp = (1023 -panel.readPot1()) * MAX_DELAY_MS / 1023;
  if (tmp < (OldDelayMs - offset) || tmp > (OldDelayMs + offset))
  {
    panel.showDelay(tmp);
    return tmp;
  }
  panel.showDelay(OldDelayMs);
  return OldDelayMs;
}


void setup() {
  Serial.begin(115200);
  while (!Serial);

  panel.beginDisplay();
  panel.showMode("Manual");
}

void loop() {
  panel.setLedError(error);

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

  knifeDelayMs = calculateKnifeDelayMs(knifeDelayMs);

  panel.showCurrent(6.9);
  panel.showVoltage(12);

  switch (currentState) {
    case State::WAITING_FOR_START:
      // Wait for start condition
      if (true)  // Az prijde prikaz knife enable
      {
        startTime = millis();
        switch (currentMode)
        {
        case Modes::MANUAL:
          delayTime = knifeDelayMs;
          currentState = State::DELAY;
          break;

        case Modes::RANDOM:
          delayTime = random(0, MAX_DELAY_MS);
          currentState = State::DELAY;
          break; 

        default:
          if (random(0, 100) < 90)
          {
            delayTime = knifeDelayMs;
            currentState = State::DELAY;
          }
          else
          {
            // Skip activation
            currentState = State::WAITING_FOR_START;
          }
          break;
        }

      }
      break;

    case State::DELAY:
       if (millis() - startTime >= delayTime)
       {
          solenoid.KnifesActive(true);
          panel.setLedStatus(true);
          currentState = State::ACTIVATE_KNIFES;
       }
      break;

    case State::ACTIVATE_KNIFES:
      if (false) // Az odpadne signal knifes enable
      {
        solenoid.KnifesActive(false);
        panel.setLedStatus(false);
        currentState = State::WAITING_FOR_START;
      }

      break;    
  }
  


}