#include "LedRGBControl.h"

LedRGBControl::LedRGBControl(int redPin, int greenPin, int bluePin) 
  : _redPin(redPin), _greenPin(greenPin), _bluePin(bluePin),
    _currentBrightness(0), 
    _isLightOn(false),
    _currentColorMode(MODE_WHITE) {
}

void LedRGBControl::begin() {
    pinMode(_redPin, OUTPUT);
    pinMode(_greenPin, OUTPUT);
    pinMode(_bluePin, OUTPUT);
    
    _applyLedColor();
}

void LedRGBControl::_applyLedColor() {
    int r = 0, g = 0, b = 0;

    if (_isLightOn) {
        if (_currentColorMode == MODE_WHITE) {
            r = _currentBrightness;
            g = _currentBrightness;
            b = _currentBrightness;
        } else if (_currentColorMode == MODE_YELLOW) {
            r = _currentBrightness;
            g = _currentBrightness;
            b = 0;
        }
    }
    
    analogWrite(_redPin, r);
    analogWrite(_greenPin, g);
    analogWrite(_bluePin, b);
}

void LedRGBControl::turnOn() {
    if (!_isLightOn || _currentBrightness == 0) {
        _currentBrightness = 50;
        _isLightOn = true;
        _applyLedColor();
    }
}

void LedRGBControl::turnOff() {
    if (_isLightOn || _currentBrightness > 0) {
        _currentBrightness = 0;
        _isLightOn = false;
        _applyLedColor();
    }
}

void LedRGBControl::increaseBrightness() {
    if (!_isLightOn && _currentBrightness == 0) {
        turnOn();
        return;
    }
    for (int b = _currentBrightness; b <= 255; b += 5) {
        _currentBrightness = b;
        _isLightOn = true;
        _applyLedColor();
        delay(50);
    }
}

void LedRGBControl::decreaseBrightness() {
    for (int b = _currentBrightness; b >= 0; b -= 5) {
        _currentBrightness = b;
        _applyLedColor();
        if (_currentBrightness == 0) {
            _isLightOn = false;
        }
        delay(50);
    }
}

void LedRGBControl::setBrightness(int brightness) {
    brightness = constrain(brightness, 0, 255);
    _currentBrightness = brightness;
    
    if (brightness > 0) {
        _isLightOn = true;
    } else {
        _isLightOn = false;
    }
    _applyLedColor();
}

void LedRGBControl::setColorMode(LedColorMode mode) {
    _currentColorMode = mode;
    if (_isLightOn) {
        _applyLedColor();
    }
}

bool LedRGBControl::isLightOn() const {
    return _isLightOn;
}

int LedRGBControl::getCurrentBrightness() const {
    return _currentBrightness;
}