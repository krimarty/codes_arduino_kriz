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

enum status {
  CLOSING,
  OPENING,
};

enum pos {
  OPEN,
  CLOSE,
  BETWEEN,
  ERROR,
};


class SolenoidModule {
    private:
    uint8_t _FbPinOpen;
    uint8_t _FbPinClose;
    uint8_t _ImeasPin;
    status status_of_knifes;
    pos position_of_knifes;

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

    status getStatusOfKnifes() const
    {
        return status_of_knifes;
    }

    pos getPositionOfKnifes() const
    {
        return position_of_knifes;
    }

    void knifeClosing()
    {
        status_of_knifes = CLOSING;
        KnifesBeetween();
    }

    void knifeOpening()
    {
        status_of_knifes = OPENING;
        KnifesBeetween();
    }

    void KnifesOpen()
    {
        position_of_knifes = OPEN;
        digitalWrite(_FbPinOpen, activeLevel(true));
        digitalWrite(_FbPinClose, activeLevel(false));
    }

    void KnifesClose()
    {
        position_of_knifes = CLOSE;
        digitalWrite(_FbPinOpen, activeLevel(false));
        digitalWrite(_FbPinClose, activeLevel(true));
    }

    void KnifesBeetween()
    {
        position_of_knifes = BETWEEN;
        digitalWrite(_FbPinOpen, activeLevel(false));
        digitalWrite(_FbPinClose, activeLevel(false));
    }

    void KnifesError()
    {
        position_of_knifes = ERROR;
        digitalWrite(_FbPinOpen, activeLevel(true));
        digitalWrite(_FbPinClose, activeLevel(true));
    }

};
