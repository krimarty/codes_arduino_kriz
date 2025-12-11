/* =============================================================
 * Project:   impact_driver_controller
 * File:      Control_panel_display.h
 * Author:    Martin Kriz
 * Company:   Ullmanna s.r.o.
 * Created:   2025-09-28
 * -------------------------------------------------------------
 * Description:
 * SH1106 display version
 * ============================================================= */

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Wire.h>

// Pin assignment
constexpr int DEFAULT_POT1_PIN       = 27;
constexpr int DEFAULT_SW1_PIN        = 18;
constexpr int DEFAULT_LED_STATUS_PIN = 17;
constexpr int DEFAULT_LED_ERROR_PIN  = 16;

class ControlPanel {
private:
    // SH1106 má šířku bufferu 132 px
    Adafruit_SH1106G _display;
    uint8_t _i2cAddr;

    uint8_t _pot1Pin;
    uint8_t _sw1Pin;
    uint8_t _ledStatusPin;
    uint8_t _ledErrorPin;

    volatile bool _sw1Pressed = false;

    static ControlPanel* _instance;

public:
    ControlPanel(
        uint8_t pot1Pin       = DEFAULT_POT1_PIN,
        uint8_t sw1Pin        = DEFAULT_SW1_PIN,
        uint8_t ledStatusPin  = DEFAULT_LED_STATUS_PIN,
        uint8_t ledErrorPin   = DEFAULT_LED_ERROR_PIN,
        uint8_t displayWidth  = 128,
        uint8_t displayHeight = 64,
        uint8_t i2c_addr      = 0x3C
    )
        // SH1106G očekává buffer 132×64
        : _display(132, displayHeight, &Wire, -1),
          _i2cAddr(i2c_addr),
          _pot1Pin(pot1Pin),
          _sw1Pin(sw1Pin),
          _ledStatusPin(ledStatusPin),
          _ledErrorPin(ledErrorPin)
    {
        pinMode(_pot1Pin, INPUT);
        pinMode(_sw1Pin, INPUT_PULLUP);
        pinMode(_ledStatusPin, OUTPUT);
        pinMode(_ledErrorPin, OUTPUT);
        digitalWrite(_ledStatusPin, LOW);
        digitalWrite(_ledErrorPin, LOW);

        _instance = this;
        attachInterrupt(digitalPinToInterrupt(_sw1Pin), handleInterruptSw1, FALLING);
    }

    void beginDisplay() {
        if (!_display.begin(_i2cAddr, true)) {
            Serial.println(F("SH1106 allocation failed"));
            for(;;);
        }
        _display.clearDisplay();
        _display.setRotation(0);
        _display.display();
    }

    void showCurrent(float current) {
    _display.fillRect(0, 0, 128, 10, SH110X_BLACK);

    _display.setTextSize(1);
    _display.setTextColor(SH110X_WHITE);
    _display.setCursor(0, 0);
    _display.print("Current: ");
    _display.print(current, 1);
    _display.println(" A");
    _display.display();
    }

    void showVoltage(float voltage) {
        _display.fillRect(0, 12, 128, 10, SH110X_BLACK);

        _display.setTextSize(1);
        _display.setTextColor(SH110X_WHITE);
        _display.setCursor(0, 12);
        _display.print("Voltage: ");
        _display.print(voltage, 2);
        _display.println(" V");
        _display.display();
    }

    void showDelay(uint32_t delayMs) {
        _display.fillRect(0, 24, 128, 10, SH110X_BLACK);

        _display.setTextSize(1);
        _display.setTextColor(SH110X_WHITE);
        _display.setCursor(0, 24);
        _display.print("Delay: ");
        _display.print(delayMs);
        _display.println(" ms");

        _display.display();
    }


    void showMode(const char* mode) {
        _display.fillRect(0, 36, 128, 10, SH110X_BLACK);

        _display.setTextSize(1);
        _display.setTextColor(SH110X_WHITE);
        _display.setCursor(0, 36);
        _display.print("Mode: ");
        _display.print(mode);

        _display.display();
    }

    void clearDisplay() {
        _display.clearDisplay();
        _display.display();
    }

    int readPot1() const { return analogRead(_pot1Pin); }

    static void handleInterruptSw1() { 
        if (_instance) _instance->_sw1Pressed = true; 
    }

    bool wasSw1Pressed() { 
        if (_sw1Pressed) { _sw1Pressed = false; return true; }
        return false;
    }

    void setLedStatus(bool on) { digitalWrite(_ledStatusPin, on ? HIGH : LOW); }
    void setLedError(bool on)  { digitalWrite(_ledErrorPin, on ? HIGH : LOW); }
    void toggleLedStatus() { digitalWrite(_ledStatusPin, !digitalRead(_ledStatusPin)); }
    void toggleLedError()  { digitalWrite(_ledErrorPin, !digitalRead(_ledErrorPin)); }
};

ControlPanel* ControlPanel::_instance = nullptr;
