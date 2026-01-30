/* =============================================================
 * Project:   impact_driver_controller
 * File:      Control_panel.h
 * Author:    Martin Kriz
 * Company:   Ullmanna s.r.o.
 * Created:   2025-09-26
 * -------------------------------------------------------------
 * Description:
 * Notes:
 * ============================================================= */

#pragma once
#include <Arduino.h>

// Pin assignment
constexpr int DEFAULT_POT1_PIN       = 27;
constexpr int DEFAULT_POT2_PIN       = 26;
constexpr int DEFAULT_SW1_PIN        = 18;
constexpr int DEFAULT_SW2_PIN        = 5;
constexpr int DEFAULT_LED_STATUS_PIN = 17;
constexpr int DEFAULT_LED_ERROR_PIN  = 16;


class ControlPanel {
private:
    uint8_t _pot1Pin;
    uint8_t _pot2Pin;
    uint8_t _sw1Pin;
    uint8_t _sw2Pin;
    uint8_t _ledStatusPin;
    uint8_t _ledErrorPin;


    volatile bool _sw1Pressed = false;
    volatile bool _sw2Pressed = false;
    static ControlPanel* _instance;

public:
    ControlPanel(        
        uint8_t pot1Pin       = DEFAULT_POT1_PIN,
        uint8_t pot2Pin       = DEFAULT_POT2_PIN,
        uint8_t sw1Pin        = DEFAULT_SW1_PIN,
        uint8_t sw2Pin        = DEFAULT_SW2_PIN,
        uint8_t ledStatusPin  = DEFAULT_LED_STATUS_PIN,
        uint8_t ledErrorPin   = DEFAULT_LED_ERROR_PIN
    )
        : _pot1Pin(pot1Pin),
        _pot2Pin(pot2Pin),
        _sw1Pin(sw1Pin),
        _sw2Pin(sw2Pin),
        _ledStatusPin(ledStatusPin),
        _ledErrorPin(ledErrorPin)
    {
        pinMode(_pot1Pin, INPUT);
        pinMode(_pot2Pin, INPUT);
        pinMode(_sw1Pin, INPUT_PULLUP);
        pinMode(_sw2Pin, INPUT_PULLUP);
        pinMode(_ledStatusPin, OUTPUT);
        pinMode(_ledErrorPin, OUTPUT);
        digitalWrite(_ledStatusPin, LOW);
        digitalWrite(_ledErrorPin, LOW);

        _instance = this;
        attachInterrupt(digitalPinToInterrupt(_sw1Pin), handleInterruptSw1, FALLING);
        attachInterrupt(digitalPinToInterrupt(_sw2Pin), handleInterruptSw2, FALLING);
    }

    static void handleInterruptSw1() { if (_instance) _instance->_sw1Pressed = true; }
    static void handleInterruptSw2() { if (_instance) _instance->_sw2Pressed = true; }

    bool wasSw1Pressed() { 
        if (_sw1Pressed) { _sw1Pressed = false; return true; } 
        return false; 
    }
    bool wasSw2Pressed() { 
        if (_sw2Pressed) { _sw2Pressed = false; return true; } 
        return false; 
    }

    int readPot1() const { return analogRead(_pot1Pin); }
    int readPot2() const { return analogRead(_pot2Pin); }

    void setLedStatus(bool on) { digitalWrite(_ledStatusPin, on ? HIGH : LOW); }
    void setLedError(bool on)  { digitalWrite(_ledErrorPin, on ? HIGH : LOW); }

    void toggleLedStatus() { digitalWrite(_ledStatusPin, !digitalRead(_ledStatusPin)); }
    void toggleLedError()  { digitalWrite(_ledErrorPin, !digitalRead(_ledErrorPin)); }
};

ControlPanel* ControlPanel::_instance = nullptr;
