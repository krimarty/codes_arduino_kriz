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

#include "include/DRV8874_motor.h"
#include "include/Control_panel.h"

constexpr long unsigned int  RUNNING_TIME = 5000; // [ms] default running time

constexpr long unsigned int  DELAY_MS = 10; // [ms] speed of main loop

DRV8874_control motor1(DEFAULT_PH_PIN, DEFAULT_EN_PIN, 9);
DRV8874_control motor2(DEFAULT_PH_PIN, DEFAULT_EN_PIN, 22);
DRV8874_control motor3(DEFAULT_PH_PIN, DEFAULT_EN_PIN, 1);
DRV8874_control motor4(DEFAULT_PH_PIN, DEFAULT_EN_PIN, 2);
DRV8874_control motor5(DEFAULT_PH_PIN, DEFAULT_EN_PIN, 3);
DRV8874_control motor6(DEFAULT_PH_PIN, DEFAULT_EN_PIN, 6);

ControlPanel panel;

// State machine
enum class State {
  STOP,
  SELECT,
  RUNNING,
  SLOWDOWN,
};
State currentState = State::SELECT;

bool stop = false;
int selectIndex = 0;
unsigned long startTime = 0;

void setup() {
  //Serial.begin(115200);
  Serial.println("Starting Motor class test...");

  motor1.begin();
  motor2.begin();
  motor3.begin();
  motor4.begin();
  motor5.begin();
  motor6.begin();

  motor1.sleep();
  motor2.sleep();
  motor3.sleep();
  motor4.sleep();
  motor5.sleep();
  motor6.sleep();

  Serial.println("Motors initialized.");
}

void loop() {
  delay(DELAY_MS);
  motor1.dutyUpdate();

  //STATUS FROM CONTROL PANEL
  if(panel.wasSw1Pressed()) 
  {
    Serial.println("Pressed START");
    stop = false;
  }

  if(panel.wasSw2Pressed()) 
  {
    Serial.println("Pressed STOP");
    currentState = State::SLOWDOWN;
    stop = true;
  }

  int duty = ((1023 -panel.readPot1()) * 100) / 1023;
  int running_time_ms = RUNNING_TIME;
  //int running_time_ms = map(panel.readPot2(), 1023, 0, 1, 360000);



  switch (currentState)
  {
    case State::STOP:
      Serial.println("State: STOP");
      motor1.stop();
      motor1.sleep();
      motor2.sleep();
      motor3.sleep();
      motor4.sleep();
      motor5.sleep();
      motor6.sleep();

      if(stop == false) 
      {
        Serial.println("State: STOP -> SELECT");
        currentState = State::SELECT;
      }
      break;

    case State::SELECT:
      Serial.println("State: SELECT");
      if (selectIndex == 0)
      {
        motor1.wake();
        motor4.wake();
        ++selectIndex;
      }
      else if (selectIndex == 1)
      {
        motor2.wake();
        motor5.wake();
        ++selectIndex;
      }
      else
      {
        motor3.wake();
        motor6.wake();
        selectIndex = 0;
      }
      startTime = millis();
      currentState = State::RUNNING;
      break;

    case State::RUNNING:
      if (millis() - startTime < running_time_ms)
      {
        Serial.println("State: RUNNING");
        motor1.left(duty);
      }
      else
      {
        currentState = State::SLOWDOWN;
      }
      break;

    case State::SLOWDOWN:
      Serial.println("State: SLOWDOWN");
      motor1.stop();
      if (motor1.getCurrentDuty() == 0)
      {
        currentState = State::STOP;
      }
      break;
  }
}