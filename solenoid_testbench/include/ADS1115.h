#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

// Pin assignment
constexpr int VOLT_DIVIDER_R1   = 10000;    // OHM
constexpr int VOLT_DIVIDER_R2   = 3300;     // OHM

class ADS1115Module {
private:
    Adafruit_ADS1115 ads;

public:
    ADS1115Module() {}

    void begin() {
        ads.begin(0x48);   // tvoje adresa
        ads.setGain(GAIN_ONE);  // ±4.096 V
    }

    // ---------------------------
    // A0 - A1 → voltage_camera_box
    // ---------------------------
    int16_t readVoltageCameraBoxRaw() {
        return ads.readADC_Differential_0_1();
    }

    float readVoltageCameraBox() {
        return rawToVoltage(readVoltageCameraBoxRaw());
    }

    // ---------------------------
    // A2 - A1 → shunt voltage
    // ---------------------------
    int16_t readShuntRaw() {
        return 0;//ads.readADC_Differential_2_1();
    }

    float readShuntVoltage() {
        return rawToVoltage(readShuntRaw());
    }

    // ---------------------------
    // Převod raw → Volty
    // ---------------------------
    float rawToVoltage(int16_t raw) {
        // GAIN_ONE → 4.096 V full-scale
        float tmp = (float)raw * (4.096f / 32768.0f);
        return tmp * ( (float)(VOLT_DIVIDER_R1 + VOLT_DIVIDER_R2) / (float)VOLT_DIVIDER_R2 );
    }
};
