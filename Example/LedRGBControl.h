#ifndef LedRGBControl_h
#define LedRGBControl_h

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <PubSubClient.h>

class LedRGBControl {
public:
    LedRGBControl(uint8_t pin, uint16_t numPixels);
    void begin();
    void turnOn();
    void increaseBrightness();
    void decreaseBrightness();
    void turnOff();
    void updateMQTTStatus(PubSubClient &client, const char* topic);
    
private:
    Adafruit_NeoPixel strip;
    int brightness;
    bool isOn;
};

#endif
