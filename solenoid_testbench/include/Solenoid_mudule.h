#pragma once
#include <Arduino.h>

static constexpr bool ACTIVE_HIGH = true;   // true = active-low, false = active-high

// Pin assignment
constexpr int DEFAULT_FB_PIN1         = 21; //open
constexpr int DEFAULT_FB_PIN2         = 20; //close
constexpr int DEFAULT_IMEAS_PIN      = 28;

constexpr float VREF        = 3.3;
constexpr int   ADC_RES     = 1023;
constexpr float SENSOR_ZERO = 0.1 * VREF;
constexpr float SENSITIVITY = 0.2;    // TMCS1107A3U 200 mV/A

class SolenoidModule {
private:
    uint8_t _FbPinOpen;
    uint8_t _FbPinClose;
    uint8_t _ImeasPin;

    static constexpr uint8_t activeLevel(bool active)
    {
        if constexpr (ACTIVE_HIGH)
            return active ? LOW : HIGH;
        else
            return active ? HIGH : LOW;
    }

public:
    SolenoidModule(uint8_t FbPinOpen = DEFAULT_FB_PIN1, uint8_t FbPinClose = DEFAULT_FB_PIN2,  uint8_t ImeasPin = DEFAULT_IMEAS_PIN) 
        : _FbPinOpen(FbPinOpen), _FbPinClose(FbPinClose),  _ImeasPin(ImeasPin)
    {
        pinMode(_FbPinOpen , OUTPUT);
        pinMode(_FbPinClose, OUTPUT);
        pinMode(_ImeasPin, INPUT);
    }

    int readRaw() const {
        return analogRead(_ImeasPin);
    }

    float readVoltage() const {
        return (analogRead(_ImeasPin) * VREF) / ADC_RES;
    }

    float readCurrent() const {
        float voltage = readVoltage();
        return (voltage - SENSOR_ZERO) / SENSITIVITY;
    }

    void KnifesOpen()
    {
        digitalWrite(_FbPinOpen, activeLevel(true));
        digitalWrite(_FbPinClose, activeLevel(false));
    }

    void KnifesClose()
    {
        digitalWrite(_FbPinOpen, activeLevel(false));
        digitalWrite(_FbPinClose, activeLevel(true));
    }

    void KnifesBeetween()
    {
        digitalWrite(_FbPinOpen, activeLevel(false));
        digitalWrite(_FbPinClose, activeLevel(false));
    }

    void KnifesError()
    {
        digitalWrite(_FbPinOpen, activeLevel(true));
        digitalWrite(_FbPinClose, activeLevel(true));
    }

};
