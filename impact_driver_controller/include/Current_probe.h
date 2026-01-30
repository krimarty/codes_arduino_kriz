/* =============================================================
 * Project:   impact_driver_controller
 * File:      Current_probe.h
 * Author:    Martin Kriz
 * Company:   Ullmanna s.r.o.
 * Created:   2025-09-28
 * -------------------------------------------------------------
 * Description:
 * Notes:
 * ============================================================= */

#pragma once
#include <Arduino.h>

// Pin assignment
constexpr int DEFAULT_IMEAS_PIN = 28;

constexpr float VREF        = 5;
constexpr int   ADC_RES     = 4095;
constexpr float SENSOR_ZERO = VREF / 2;
constexpr float SENSITIVITY = 0.1;      // V/A (ACS712 20A → cca 100 mV/A)

class CurrentProbe {
private:
    uint8_t _ImeasPin;

public:
    CurrentProbe(uint8_t ImeasPin = DEFAULT_IMEAS_PIN) : _ImeasPin(ImeasPin)
    {
        pinMode(_ImeasPin, INPUT);
    }

    // Vrací syrovou ADC hodnotu (0–4095)
    int readRaw() const {
        return analogRead(_ImeasPin);
    }

    // Vrací napětí na senzoru (0–3.3 V)
    float readVoltage() const {
        return (analogRead(_ImeasPin) * VREF) / ADC_RES;
    }

    // Vrací proud v ampérech
    float readCurrent() const {
        float voltage = readVoltage();
        return (voltage - SENSOR_ZERO) / SENSITIVITY;
    }
};

