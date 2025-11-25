#include "UserInterface.h"

UserInterface ui;

unsigned long lastLedTest = 0;
int ledState = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);  // Wait for Serial connection (useful for boards like Raspberry Pi Pico)
  Serial.println("Starting UserInterface hardware test...");
}

void loop() {
  // === Read and print user input states ===
  Direction dir = ui.getDirection();
  String dirStr = (dir == Direction::LEFT) ? "LEFT" :
                  (dir == Direction::RIGHT) ? "RIGHT" : "STOP";

  ModeType mode = ui.getModeType();
  String modeStr = (mode == ModeType::MANUAL) ? "MANUAL" :
                   (mode == ModeType::SEQUENCE) ? "SEQUENCE" : "RANDOM";

  float potPercent = ui.readFilteredPotPercent();

  Serial.print("Direction: "); Serial.print(dirStr);
  Serial.print(" | Mode: "); Serial.print(modeStr);
  Serial.print(" | Potentiometer: "); Serial.print(potPercent, 1); Serial.println(" %");

  if (ui.isSyncMode()) {Serial.println("Syncmode active");}
  else  {Serial.println("Syncmode off");}

  if (ui.isInverse()) {Serial.println("Inversemode active");}
  else  {Serial.println("Inversemode off");}

  // === LED test: cycle through all LED outputs every 1000ms ===
  if (millis() - lastLedTest >= 1000) {
    lastLedTest = millis();
    ledState = (ledState + 1) % 4; // States: 0 → 3

    switch (ledState) {
      case 0:
        ui.setSignalLed(false);
        ui.setLeftLed(false);
        ui.setRightLed(false);
        Serial.println("LED TEST: All LEDs OFF");
        break;
      case 1:
        ui.setSignalLed(true);
        ui.setLeftLed(false);
        ui.setRightLed(false);
        Serial.println("LED TEST: Signal LED ON");
        break;
      case 2:
        ui.setSignalLed(false);
        ui.setLeftLed(true);
        ui.setRightLed(false);
        Serial.println("LED TEST: LEFT LED ON");
        break;
      case 3:
        ui.setSignalLed(false);
        ui.setLeftLed(false);
        ui.setRightLed(true);
        Serial.println("LED TEST: RIGHT LED ON");
        break;
    }
  }

  delay(200); // Short delay for better responsiveness
}
