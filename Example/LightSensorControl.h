// LightSensorControl.h
#ifndef LIGHT_SENSOR_CONTROL_H
#define LIGHT_SENSOR_CONTROL_H

#include <Arduino.h>

class LightSensorControl {
public:
    // Constructor: Khởi tạo đối tượng với các chân Analog và Digital
    LightSensorControl(int analogPin, int digitalPin);

    // Khởi tạo chân cảm biến
    void begin();

    // Đọc giá trị analog từ cảm biến (0-4095 cho ESP32 ADC 12-bit)
    int readAnalogValue();

    // Đọc giá trị digital từ cảm biến (HIGH/LOW)
    int readDigitalValue();

private:
    int _analogPin;
    int _digitalPin;
};

#endif