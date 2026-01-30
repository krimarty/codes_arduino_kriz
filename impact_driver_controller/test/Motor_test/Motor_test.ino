#include "DRV8874_motor.h"


DRV8874_control motor;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Starting Motor class test...");

  motor.begin();

  Serial.println("Motor initialized.");
}

void loop() {
  motor.wake();


  delay(500);

  // === Reverse ramp ===
  Serial.println("Turning LEFT (ramping up)...");
  motor.left(50);
  for (int i = 0; i < 210; i++) {
    motor.dutyUpdate();
    delay(100);
  }

  delay(1000);  // Hold

  // === Reverse ramp ===
  Serial.println("Stopping (ramping down)...");
  motor.left(0);
  for (int i = 0; i < 210; i++) {
    motor.dutyUpdate();
    delay(100);
  }

}
