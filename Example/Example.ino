#include <WiFi.h>
#include <PubSubClient.h>
#include "LedRGBControl.h"

#define LED_PIN 5
#define LED_COUNT 1
#define MQTT_BROKER "mqtt.flespi.io"
#define MQTT_TOPIC "esp32/led/status"

const char* ssid = "KTX KHU B";
const char* password = "6666666666";
const char* mqtt_user = "nAcJcPwx0VAuLEZKZH0r8YZM8surzaA0lgSJThXFDJh8TKARmhb64rTw4xjfkpZL";
const char* mqtt_pass = "";

WiFiClient espClient;
PubSubClient client(espClient);
LedRGBControl led(LED_PIN, LED_COUNT);

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");
    
    client.setServer(MQTT_BROKER, 1883);
    led.begin();
}

void loop() {
    if (!client.connected()) {
        while (!client.connect("ESP32Client", mqtt_user, mqtt_pass)) {
            delay(1000);
        }
    }
    
    led.turnOn();
    led.updateMQTTStatus(client, MQTT_TOPIC);
    delay(2000);
    
    led.increaseBrightness();
    led.updateMQTTStatus(client, MQTT_TOPIC);
    delay(2000);
    
    led.decreaseBrightness();
    led.updateMQTTStatus(client, MQTT_TOPIC);
    delay(2000);
    
    led.turnOff();
    led.updateMQTTStatus(client, MQTT_TOPIC);
    delay(2000);
}
