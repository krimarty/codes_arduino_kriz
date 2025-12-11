#pragma once
#include <Arduino.h>

// Pin assignment
constexpr int DEFAULT_FB_PIN         = 21;
constexpr int DEFAULT_ADC_READY_PIN  = 20;
constexpr int DEFAULT_IMEAS_PIN      = 28;

constexpr float VREF        = 3.3;
constexpr int   ADC_RES     = 1023;
constexpr float SENSOR_ZERO = 0.1 * VREF;
constexpr float SENSITIVITY = 0.2;    // TMCS1107A3U 200 mV/A

class SolenoidModule {
private:
    uint8_t _FbPin;
    uint8_t _AdcReadyPin;
    uint8_t _ImeasPin;

public:
    SolenoidModule(uint8_t FbPin = DEFAULT_FB_PIN, uint8_t AdcReadyPin = DEFAULT_ADC_READY_PIN, uint8_t ImeasPin = DEFAULT_IMEAS_PIN) 
        : _FbPin(FbPin), _AdcReadyPin(AdcReadyPin), _ImeasPin(ImeasPin)
    {
        pinMode(_FbPin, OUTPUT);
        pinMode(_AdcReadyPin, INPUT);
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

    void KnifesActive(bool on) { digitalWrite(_FbPin, on ? LOW : HIGH); }
};
