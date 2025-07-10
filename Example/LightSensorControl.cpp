// LightSensorControl.cpp
#include "LightSensorControl.h"
#include <Arduino.h>

// Constructor: Khởi tạo đối tượng với các chân Analog và Digital
LightSensorControl::LightSensorControl(int analogPin, int digitalPin)
  : _analogPin(analogPin), _digitalPin(digitalPin) {
    // Không có gì thêm trong constructor
}

// Khởi tạo chân cảm biến
void LightSensorControl::begin() {
    pinMode(_digitalPin, INPUT); // Chân DO là Input
    // Chân AO không cần pinMode explicit vì analogRead tự động cấu hình
    Serial.println("LightSensorControl da khoi tao.");
}

// Đọc giá trị analog từ cảm biến (0-4095 cho ESP32 ADC 12-bit)
int LightSensorControl::readAnalogValue() {
    // Đọc giá trị từ chân analog
    return analogRead(_analogPin);
}

// Đọc giá trị digital từ cảm biến (HIGH/LOW)
int LightSensorControl::readDigitalValue() {
    // Đọc giá trị từ chân digital
    return digitalRead(_digitalPin);
}