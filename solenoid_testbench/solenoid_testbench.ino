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


constexpr long unsigned int  DELAY_MS = 10; // [ms] speed of main loop

ControlPanel panel;


void setup() {
  Serial.begin(115200);
  while (!Serial);

  //Wire.setSDA(4);     // natvrdo nastavené SDA
  //Wire.setSCL(5);     // natvrdo nastavené SCL
  //Wire.begin();             // spustíme I2C na těchto pinech
  //Wire.setClock(100000);    // zpomalíme na 100 kHz (bezpečnější pro delší vodiče)
  panel.beginDisplay();
  pinMode(21, OUTPUT);
}

void loop() {

  panel.showCurrent(6.60);
  
  digitalWrite(21, HIGH);  // 1
  panel.showMode("HIGH");
  delay(2000);             // 2 s

  digitalWrite(21, LOW);   // 0
  panel.showMode("LOW");
  delay(2000);             // 2 s


}