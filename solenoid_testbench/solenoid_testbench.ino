/* =============================================================
 * Project:   impact_driver_controller
 * File:      impact_driver_controller.ino
 * Author:    Martin Kriz
 * Company:   Ullmanna s.r.o.
 * Created:   2025-09-26
 * -------------------------------------------------------------
 * Description:
 *
 * Notes: R=10k, C2=22nF
 * ============================================================= */

#include "Wire.h"
#include "include/Control_panel_display.h"
#include "include/Solenoid_mudule.h"
#include "include/ADS1115.h"

// State machine
enum class State {
  KNIFES_OPEN,
  DELAY,
  KNIFES_CLOSE,
};
State currentState = State::KNIFES_OPEN;

enum class Modes {
  PRECISE,          // precise timing (no jitter)
  JITTER,           // timing with jitter (JITTER_RANGE % around delay)
  PRECISE_BOUNCING, // precise timing + bouncing
  REAL,             // "real" + jitter + bouncing + error
};
Modes currentMode = Modes::PRECISE;

enum class Modes_Display {
  VIEW,
  SET_DELAY,
  SET_MODE,
  SUMM,
  OFF,
};
Modes_Display currentModeDisplay = Modes_Display::OFF;
constexpr float CURRENT_TRESHOLD = 0.13;  // [A] current threshold for knifes activation
constexpr unsigned int JITTER_RANGE   = 20;  // [%] jitter range for JITTER mode
constexpr unsigned int MAX_DELAY_MS   = 200; // [ms] maximum delay for knifes activation
constexpr unsigned int N_OF_BOUNCES   = 3;   // number of bounces for BOUNCING modes
constexpr unsigned int BOUNCING_TIME = 70;  // [ms] time when bouncing

ControlPanel panel;
SolenoidModule solenoid;
ADS1115Module adc;

bool error = false;
unsigned int knifeDelayMs = 0;
float solenoid_current = 0.0;

// Timer
unsigned long startTime = 0;
volatile bool alarm_fired = false;


int calculateKnifeDelayMs(int OldDelayMs)
{
  int offset = 1;
  int tmp = (1023 -panel.readPot1()) * MAX_DELAY_MS / 1023;
  if (tmp < (OldDelayMs - offset) || tmp > (OldDelayMs + offset))
  {
    return tmp;
  }
  return OldDelayMs;
}

int64_t alarm_callback(alarm_id_t id, void *user_data)
{
  alarm_fired = true;

  if (solenoid.getStatusOfKnifes() == CLOSING)
  {
    solenoid.KnifesClose();
    panel.setLedStatus(true);
    currentState = State::KNIFES_CLOSE;
  }
  else
  {
    solenoid.KnifesOpen();
    panel.setLedStatus(false);
    currentState = State::KNIFES_OPEN;
  }
  return 0;
}

void startTimerMs(uint32_t ms)
{
  if (ms == 0)
  {
    if (solenoid.getStatusOfKnifes() == CLOSING)
    {
      solenoid.KnifesClose();
      panel.setLedStatus(true);
      currentState = State::KNIFES_CLOSE;
    }
    else
    {
      solenoid.KnifesOpen();
      panel.setLedStatus(false);
      currentState = State::KNIFES_OPEN;
    }
    return;
  }
  add_alarm_in_ms(ms, alarm_callback, NULL, false);
}



void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println(F("Solenoid Testbench Starting..."));

  adc.begin();
  panel.beginDisplay();

  solenoid.KnifesOpen();
}

void loop() {
  panel.setLedError(error);

  // User 
  if (panel.wasSw1Pressed()) {display_next_mode();}
  if (currentModeDisplay != Modes_Display::OFF) {display_state_machine();}

  switch (currentMode)
  {
  case Modes::PRECISE:
    precise_mode();
    break;
  
  case Modes::JITTER:
    jitter_mode();
    break;

  case Modes::PRECISE_BOUNCING:
    precise_bouncing_mode();
    break;

  default:
    real_mode();
    break;
  }
  
}

void precise_mode()
{
  solenoid_current = solenoid.readCurrent();
  switch (currentState) {
    case State::KNIFES_OPEN:
      // Wait for start condition
      if (solenoid_current > CURRENT_TRESHOLD)
      {
        solenoid.knifeClosing();
        currentState = State::DELAY;
        startTimerMs(knifeDelayMs);
      }
      break;

    case State::KNIFES_CLOSE:
      if (solenoid_current < CURRENT_TRESHOLD) // Az odpadne signal knifes enable
      {
        solenoid.knifeOpening();
        currentState = State::DELAY;
        startTimerMs(knifeDelayMs);
      }
      break;
      
    default:
      break;
  }
}

void jitter_mode()
{
  solenoid_current = solenoid.readCurrent();
  switch (currentState) {
    case State::KNIFES_OPEN:

      // Wait for start condition
      if (solenoid_current > CURRENT_TRESHOLD)
      {
        Serial.print("Jitter delay time: "); Serial.println(getJitterDelay(knifeDelayMs));
        solenoid.knifeClosing();
        currentState = State::DELAY;
        startTimerMs(getJitterDelay(knifeDelayMs));
      }
      break;

    case State::KNIFES_CLOSE:

      if (solenoid_current < CURRENT_TRESHOLD) // Az odpadne signal knifes enable
      {
        Serial.print("Jitter delay time: "); Serial.println(getJitterDelay(knifeDelayMs));
        solenoid.knifeOpening();
        currentState = State::DELAY;
        startTimerMs(getJitterDelay(knifeDelayMs));
      }
      break;
      
    default:
      break;
  }
}
void precise_bouncing_mode()
{
  // To be implemented
}
void real_mode()
{
  // To be implemented
}

void display_next_mode()
{
  switch (currentModeDisplay)
  {
  case Modes_Display::OFF:
    currentModeDisplay = Modes_Display::VIEW;
    panel.clearDisplay();
    break;

  case Modes_Display::VIEW:
    currentModeDisplay = Modes_Display::SET_DELAY;
    panel.clearDisplay();
    break;

  case Modes_Display::SET_DELAY:
    currentModeDisplay = Modes_Display::SET_MODE;
    panel.clearDisplay();
    break;

  case Modes_Display::SET_MODE:
    currentModeDisplay = Modes_Display::SUMM;
    panel.clearDisplay();
    break;

  case Modes_Display::SUMM:
    currentModeDisplay = Modes_Display::OFF;
    panel.clearDisplay();
    break;

  default:
    currentModeDisplay = Modes_Display::OFF;
    panel.clearDisplay();
    break;
  }
}

void display_state_machine()
{
  int tmp = panel.readPot1();
  delay(10);
  switch (currentModeDisplay)
  {
  case Modes_Display::VIEW:
    show_mode();
    panel.showDelay(knifeDelayMs);
    break;

  case Modes_Display::SET_DELAY:
    knifeDelayMs = calculateKnifeDelayMs(knifeDelayMs);
    panel.showDelay(knifeDelayMs);
    break;

  case Modes_Display::SET_MODE:
    if (tmp < 256)
      currentMode = Modes::REAL;
    else if (tmp < 512)
      currentMode = Modes::PRECISE_BOUNCING;
    else if (tmp < 768)
      currentMode = Modes::JITTER;
    else
      currentMode = Modes::PRECISE;

    show_mode();
    break;

  case Modes_Display::SUMM:
    show_mode();
    panel.showDelay(knifeDelayMs);
    break;
  
  default:
    panel.clearDisplay();
    break;
  }

}

void show_mode()
{
  switch (currentMode)
  {
  case Modes::PRECISE:
    panel.showMode("Precise");
    break;
  
  case Modes::JITTER:
    panel.showMode("Jitter");
    break;

  case Modes::PRECISE_BOUNCING:
    panel.showMode("Precise Bouncing");
    break;

  default:
    panel.showMode("Real");
    break;
  }
}


unsigned int getJitterDelay(unsigned int knifeDelayMs) {
  int jitterPercent = random(-JITTER_RANGE, JITTER_RANGE + 1);
  int jitterMs = ((int)knifeDelayMs * jitterPercent) / 100;

  int result = (int)knifeDelayMs + jitterMs;

  return (unsigned int)result;
}
