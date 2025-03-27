#include "LedRGBControl.h"
LedRGBControl::LedRGBControl(uint8_t pin, uint16_t numPixels)
    : strip(numPixels, pin, NEO_GRB + NEO_KHZ800), brightness(50), isOn(false) {}

void LedRGBControl::begin() {
    strip.begin();
    strip.show();
    strip.setBrightness(brightness);
}

void LedRGBControl::turnOn() {
    isOn = true;
    strip.setBrightness(brightness);
    for (int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(255, 255, 255));
    }
    strip.show();
}

void LedRGBControl::increaseBrightness() {
    if (brightness < 255) brightness += 5;
    strip.setBrightness(brightness);
    strip.show();
}

void LedRGBControl::decreaseBrightness() {
    if (brightness > 0) brightness -= 5;
    strip.setBrightness(brightness);
    strip.show();
}

void LedRGBControl::turnOff() {
    isOn = false;
    strip.clear();
    strip.show();
}

void LedRGBControl::updateMQTTStatus(PubSubClient &client, const char* topic) {
    String message = "{";
    message += "\"status\": \"" + String(isOn ? "ON" : "OFF") + "\",";
    message += "\"brightness\": " + String(brightness);
    message += "}";
    client.publish(topic, message.c_str());
}
