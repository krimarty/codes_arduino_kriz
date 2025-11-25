/* =============================================================
 * Project:   DRV8874_motor
 * File:      DRV8874_motor.h
 * Author:    Martin Kriz
 * Company:   Ullmanna s.r.o.
 * Created:   2025-09-26
 * -------------------------------------------------------------
 * Description:

 * Notes:
 * ============================================================= */

#pragma once
#include <Arduino.h>
#include <array>
#include <vector>


// Default pin assignment
constexpr uint8_t DEFAULT_EN_PIN    = 7;   // PWM pin
constexpr uint8_t DEFAULT_PH_PIN    = 8;   // Direction pin
constexpr uint8_t DEFAULT_SLEEP_PIN = 9;   // Sleep pin
const std::vector<uint8_t> DEFAULT_FAULT_PINS = {10, 11, 12, 13, 14, 15};

// DRV8874_control
class DRV8874_control {
private:
    uint8_t _phPin, _enPin, _sleepPin, _faultPin;
    int16_t _currentDuty = 0;
    int16_t _targetDuty = 0;
    uint32_t _lastUpdate = 0;
    uint16_t _rampDelay = 10;

    bool _isInverse = false;

    void setPwmFreq(uint32_t freq = 25000) {
        analogWriteFreq(freq);
    }

    void dutyUpdate_worker() {
        if (_currentDuty == _targetDuty) return;
        if (millis() - _lastUpdate >= _rampDelay) {
            if (_currentDuty < _targetDuty) {
                _currentDuty++;
            } else {
                _currentDuty--;
            }
            if (_currentDuty < 0) {
                digitalWrite(_phPin, LOW);
            } else if (_currentDuty > 0) {
                digitalWrite(_phPin, HIGH);
            }
            analogWrite(_enPin, map(abs(_currentDuty), 0, 100, 0, 255));
            _lastUpdate = millis();
        }
    }

public:
    DRV8874_control(
        uint8_t phPin = DEFAULT_PH_PIN, 
        uint8_t enPin = DEFAULT_EN_PIN, 
        uint8_t sleepPin = DEFAULT_SLEEP_PIN) 
        : _phPin(phPin), _enPin(enPin), _sleepPin(sleepPin) 
    {
        pinMode(_phPin, OUTPUT);
        pinMode(_enPin, OUTPUT);
        pinMode(_sleepPin, OUTPUT);

        digitalWrite(_phPin, LOW);
        digitalWrite(_enPin, LOW);
        digitalWrite(_sleepPin, HIGH);
    }

    void begin() {
        pinMode(_phPin, OUTPUT);
        pinMode(_enPin, OUTPUT);
        pinMode(_sleepPin, OUTPUT);
        setPwmFreq();
        pinMode(_faultPin, INPUT);
        wake();
        enable();
        stop();
    }

    void dutyUpdate() {
        dutyUpdate_worker();
    }

    void wake() {
        digitalWrite(_sleepPin, HIGH);
    }

    void sleep() {
        digitalWrite(_sleepPin, LOW);
    }

    void enable() {
        digitalWrite(_sleepPin, HIGH); 
    }

    void disable() {
        digitalWrite(_sleepPin, LOW);
    }

    void left(int16_t duty) {
        if (duty > 100) {duty = 100;}
        _targetDuty = -duty;
    }
    
    void right(int16_t duty) {
        if (duty > 100) {duty = 100;}
        _targetDuty = duty;
    }
    
    void stop() {
        _targetDuty = 0;
    }

};

class DRV8874_Indicator {
public:
    DRV8874_Indicator(const std::vector<uint8_t> &faultPins = DEFAULT_FAULT_PINS)
        : _faultPins(faultPins)
    {
        for (auto pin : _faultPins) {
            pinMode(pin, INPUT_PULLUP);
        }
    }

    bool isFault(size_t idx) {
        if (idx >= _faultPins.size()) return false;
        return digitalRead(_faultPins[idx]) == LOW;
    }

private:
    std::vector<uint8_t> _faultPins;
};


