#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

// Thông tin WiFi
#define WIFI_SSID     "KTX KHU B"
#define WIFI_PASSWORD "6666666666"

// Thông tin MQTT 
#define MQTT_SERVER   "wss://mqtt.flespi.io:443"
#define MQTT_PORT     1883
#define MQTT_USERNAME "nAcJcPwx0VAuLEZKZH0r8YZM8surzaA0lgSJThXFDJh8TKARmhb64rTw4xjfkpZL"
#define MQTT_TOPIC    "flespi/state/platform/tokens/73762982/+"

// Chân LED WS2812
#define LED_PIN    5     
#define LED_COUNT  1    

// Khai báo đối tượng LED & MQTT
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
WiFiClient espClient;
PubSubClient client(espClient);

// Kết nối WiFi
void connectWiFi() {
  Serial.print("Đang kết nối WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" Đã kết nối WiFi!");
}

// Kết nối MQTT
void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Đang kết nối MQTT...");
    if (client.connect("ESP32_Client", MQTT_USERNAME, "")) {
      Serial.println(" Đã kết nối MQTT!");
    } else {
      Serial.print(" Lỗi kết nối, thử lại sau 5s...");
      delay(5000);
    }
  }
}

// Gửi trạng thái lên MQTT
void sendMQTT(const char* message) {
  client.publish(MQTT_TOPIC, message);
  Serial.print("Gửi MQTT: ");
  Serial.println(message);
}

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.show();

  connectWiFi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  connectMQTT();
}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();

  turnOnLEDs();
  delay(2000);
  
  increaseBrightness();
  delay(2000);
  
  decreaseBrightness();
  delay(2000);
  
  turnOffLEDs();
  delay(2000);
}

// Bật đèn 
void turnOnLEDs() {
  strip.setBrightness(50);
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(255, 255, 255));  
  }
  strip.show();
  sendMQTT("ON");
}

// Tăng dần độ sáng
void increaseBrightness() {
  for (int b = 50; b <= 255; b += 5) {
    strip.setBrightness(b);
    strip.show();
    char msg[50];
    sprintf(msg, "Brightness: %d", b);
    sendMQTT(msg);
    delay(50);
  }
}

// Giảm dần độ sáng
void decreaseBrightness() {
  for (int b = 255; b >= 0; b -= 5) {
    strip.setBrightness(b);
    strip.show();
    char msg[50];
    sprintf(msg, "Brightness: %d", b);
    sendMQTT(msg);
    delay(50);
  }
}

// Tắt tất cả đèn
void turnOffLEDs() {
  strip.clear();
  strip.show();
  sendMQTT("OFF");
}
