#ifndef LED_RGB_CONTROL_H
#define LED_RGB_CONTROL_H

#include <Arduino.h>

enum LedColorMode {
    MODE_WHITE,
    MODE_YELLOW
};

class LedRGBControl {
public:
    LedRGBControl(int redPin, int greenPin, int bluePin); 

    void begin();
    void turnOn();
    void turnOff();
    void increaseBrightness();
    void decreaseBrightness();
    void setBrightness(int brightness);
    void setColorMode(LedColorMode mode);

    bool isLightOn() const;
    int getCurrentBrightness() const;
    LedColorMode getCurrentColorMode() const { return _currentColorMode; }

private:
    int _redPin;
    int _greenPin;
    int _bluePin;
    
    int _currentBrightness;
    bool _isLightOn;
    LedColorMode _currentColorMode;

    void _applyLedColor(); 
};

#endif