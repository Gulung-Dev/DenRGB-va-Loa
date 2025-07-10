#include <WiFi.h>
#include <PubSubClient.h>
#include "LedRGBControl.h"     // Bao gồm file header của thư viện LED 
#include "SpeakerControl.h"    // Bao gồm file header của thư viện Loa
#include "LightSensorControl.h" // Bao gồm file header của thư viện Cảm biến ánh sáng

// --- Thông tin WiFi  ---
#define WIFI_SSID           "Kano"
#define WIFI_PASSWORD       "123456789"

// --- Thông tin MQTT  ---
#define MQTT_SERVER         "mqtt.flespi.io" 
#define MQTT_PORT           1883 
#define MQTT_USERNAME       "ek0Y9bd0kSQSSyTyymSWVwnqijTaRqx0XnZChAsDzUjjIFEHTQlf5scB9kC9F58X"

// --- Base Topic cho tất cả trạng thái và lệnh (đã sửa cho phù hợp flespi.io) ---
#define TOKEN_BASE_PATH     "flespi/state/platform/tokens/"   // Base path cho trạng thái
#define COMMAND_BASE_PATH   "flespi/command/platform/tokens/" // Base path cho lệnh

// --- Thông tin LED ---
#define GREEN_LED_PIN       4 
#define RED_LED_PIN         16  
#define BLUE_LED_PIN        17  
#define MQTT_LIGHT_STATE_BASE_SUBTOPIC "light/" // Topic con cho trạng thái đèn
#define MQTT_LIGHT_COMMAND_TOPIC      "/light/set" // Topic lệnh cho đèn

// --- Thông tin Loa ---
#define SPEAKER_PIN         18  
#define DEFAULT_SPEAKER_VOLUME 50 // Âm lượng mặc định/cài đặt cho loa (0-100)
#define MQTT_AUDIO_STATE_BASE_SUBTOPIC "speaker/" // Topic con cho trạng thái âm thanh
#define MQTT_AUDIO_COMMAND_TOPIC      "/speaker/set" // Topic lệnh cho loa

// --- Thông tin Cảm biến ánh sáng 2WXL ---
#define LIGHT_SENSOR_AO_PIN         34 // Chân Analog Output (AO) của module 2WXL 
#define LIGHT_SENSOR_DO_PIN         25 // Chân Digital Output (DO) của module 2WXL
#define MQTT_LIGHT_SENSOR_STATE_BASE_SUBTOPIC "light_sensor/" // Topic con cho trạng thái cảm biến ánh sáng
#define SENSOR_READ_INTERVAL_MS     5000 // Đọc cảm biến mỗi 5 giây

// --- Khai báo Đối tượng ---
WiFiClient espClient;
PubSubClient client(espClient);

// Khai báo đối tượng LedRGBControl, SpeakerControl và LightSensorControl
LedRGBControl myLEDs(RED_LED_PIN, GREEN_LED_PIN, BLUE_LED_PIN);
SpeakerControl mySpeaker(SPEAKER_PIN, DEFAULT_SPEAKER_VOLUME); 
LightSensorControl myLightSensor(LIGHT_SENSOR_AO_PIN, LIGHT_SENSOR_DO_PIN);

// Biến trạng thái loa (để biết loa đang phát hay dừng)
bool isAudioPlaying = false; 

// Biến để theo dõi thời gian đọc cảm biến
unsigned long lastLightSensorReadMillis = 0;

// Biến để theo dõi trạng thái điều khiển đèn (true: tự động, false: thủ công qua MQTT)
bool autoBrightnessMode = true; 


// --- Khai báo hàm Callback ---
void callback(char* topic, byte* payload, unsigned int length);

// --- Hàm Kết nối WiFi ---
void connectWiFi() {
  Serial.print("Đang kết nối WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" Đã kết nối WiFi!");
}

// --- Hàm Gửi Trạng thái lên MQTT với Topic con và tên trạng thái cụ thể ---
void sendMQTTState(const char* deviceBaseSubTopic, const char* stateName, const char* message) {
  String fullTopic = String(TOKEN_BASE_PATH) + String(deviceBaseSubTopic) + String(stateName); 
  client.publish(fullTopic.c_str(), message);
  Serial.print("Gửi MQTT - Topic: ");
  Serial.print(fullTopic);
  Serial.print(", Message: ");
  Serial.println(message);
}

// --- Hàm gửi tất cả trạng thái hiện tại của đèn lên MQTT ---
void sendLEDState() {
    sendMQTTState(MQTT_LIGHT_STATE_BASE_SUBTOPIC, "power", myLEDs.isLightOn() ? "ON" : "OFF");
    sendMQTTState(MQTT_LIGHT_STATE_BASE_SUBTOPIC, "brightness", String(myLEDs.getCurrentBrightness()).c_str());
    String colorModeStr = (myLEDs.getCurrentColorMode() == MODE_WHITE) ? "WHITE" : "YELLOW";
    sendMQTTState(MQTT_LIGHT_STATE_BASE_SUBTOPIC, "color_mode", colorModeStr.c_str());
    sendMQTTState(MQTT_LIGHT_STATE_BASE_SUBTOPIC, "auto_mode", autoBrightnessMode ? "ON" : "OFF");
}

// --- Hàm gửi trạng thái loa lên MQTT ---
void sendAudioState() {
    sendMQTTState(MQTT_AUDIO_STATE_BASE_SUBTOPIC, "status", isAudioPlaying ? "PLAYING" : "STOPPED");
    sendMQTTState(MQTT_AUDIO_STATE_BASE_SUBTOPIC, "volume", String(mySpeaker.getVolume()).c_str()); 
}

// --- Hàm Kết nối MQTT ---
void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Đang kết nối MQTT...");
    String clientId = "ESP32_Client_";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str(), MQTT_USERNAME, "")) {
      Serial.println(" Đã kết nối MQTT!");
      client.setCallback(callback);
      
      // Đăng ký lắng nghe lệnh cho ĐÈN và LOA
      client.subscribe(MQTT_LIGHT_COMMAND_TOPIC); 
      Serial.print("Đã đăng ký topic điều khiển đèn: ");
      Serial.println(MQTT_LIGHT_COMMAND_TOPIC);

      client.subscribe(MQTT_AUDIO_COMMAND_TOPIC);         
      Serial.print("Đã đăng ký topic điều khiển âm thanh: ");
      Serial.println(MQTT_AUDIO_COMMAND_TOPIC);
      
      // Gửi trạng thái ban đầu của cả hai thiết bị khi kết nối lại
      sendLEDState(); 
      sendAudioState(); 
    } else {
      Serial.print(" Lỗi kết nối MQTT, Mã lỗi: ");
      Serial.print(client.state());
      Serial.println(". Thử lại sau 5s...");
      delay(5000);
    }
  }
}

// --- Hàm Callback xử lý tin nhắn MQTT nhận được  ---
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Tin nhắn nhận được [");
  Serial.print(topic);
  Serial.print("] ");
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // --- Xử lý lệnh cho ĐÈN ---
  if (String(topic) == MQTT_LIGHT_COMMAND_TOPIC) {
    // Nếu có lệnh điều khiển thủ công, tắt chế độ tự động
    // Trừ khi lệnh đó là bật chế độ tự động
    if (message != "AUTO_ON") {
        autoBrightnessMode = false; 
    }

    if (message == "ON") {
      myLEDs.turnOn();
      Serial.println("Đã nhận lệnh BẬT đèn.");
    } else if (message == "OFF") {
      myLEDs.turnOff();
      Serial.println("Đã nhận lệnh TẮT đèn.");
    } else if (message.startsWith("BRIGHTNESS:")) {
      int brightness = message.substring(11).toInt();
      brightness = constrain(brightness, 0, 255);
      myLEDs.setBrightness(brightness); 
      Serial.print("Đã đặt độ sáng: ");
      Serial.println(brightness);
    } else if (message.startsWith("COLOR:")) {
        String modeString = message.substring(6);
        if (modeString == "WHITE") {
            myLEDs.setColorMode(MODE_WHITE);
            Serial.println("Đã đặt chế độ màu: TRẮNG");
        } else if (modeString == "YELLOW") {
            myLEDs.setColorMode(MODE_YELLOW);
            Serial.println("Đã đặt chế độ màu: VÀNG");
        } else {
            Serial.println("Chế độ màu không hợp lệ. Chỉ chấp nhận WHITE hoặc YELLOW.");
        }
    } else if (message == "AUTO_BRIGHTNESS_ON") {
        autoBrightnessMode = true;
        Serial.println("Đã bật chế độ điều chỉnh độ sáng tự động.");
    } else if (message == "AUTO_BRIGHTNESS_OFF") {
        autoBrightnessMode = false;
        Serial.println("Đã tắt chế độ điều chỉnh độ sáng tự động.");
    }
    sendLEDState(); // Gửi trạng thái mới của đèn lên MQTT sau khi xử lý lệnh
  }

  // --- Xử lý lệnh cho LOA ---
  else if (String(topic) == MQTT_AUDIO_COMMAND_TOPIC) {
    if (message == "PLAY") { 
      isAudioPlaying = true; 
      mySpeaker.play(); 
      Serial.println("Đã nhận lệnh PHÁT NHẠC.");
    } else if (message == "STOP") {
      mySpeaker.stopPlaying(); 
      isAudioPlaying = false; 
      Serial.println("Đã nhận lệnh DỪNG NHẠC.");
    } else if (message.startsWith("VOLUME:")) { 
        int volume = message.substring(7).toInt();
        volume = constrain(volume, 0, 100); 
        mySpeaker.setVolume(volume); 
        Serial.print("Đã đặt âm lượng: ");
        Serial.println(volume);
    }
    sendAudioState(); // Gửi trạng thái mới của loa lên MQTT sau khi xử lý lệnh
  }
}

// ======================= Hàm Setup (Chạy một lần khi khởi động) =======================
void setup() {
  Serial.begin(115200); // Đã sửa lại baud rate phổ biến 115200
  Serial.println("Khoi tao he thong thong minh...");
  
  // Khởi tạo cả đèn, loa và cảm biến ánh sáng
  myLEDs.begin(); 
  mySpeaker.begin(); 
  myLightSensor.begin();
  
  connectWiFi();
  
  client.setServer(MQTT_SERVER, MQTT_PORT);
  randomSeed(micros());
  connectMQTT();
}

// ======================= Hàm Loop =======================
void loop() {
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();
  
  // Điều chỉnh độ sáng đèn theo cảm biến ánh sáng
  if (autoBrightnessMode && (millis() - lastLightSensorReadMillis >= SENSOR_READ_INTERVAL_MS)) {
    int ambientLightValue = myLightSensor.readAnalogValue(); // Đọc giá trị cường độ ánh sáng môi trường

    Serial.print("Cường độ ánh sáng môi trường (Analog): ");
    Serial.println(ambientLightValue);

    // Gửi giá trị cảm biến lên MQTT
    sendMQTTState(MQTT_LIGHT_SENSOR_STATE_BASE_SUBTOPIC, "analog_value", String(ambientLightValue).c_str());
    sendMQTTState(MQTT_LIGHT_SENSOR_STATE_BASE_SUBTOPIC, "digital_state", String(myLightSensor.readDigitalValue()).c_str());

    // Điều chỉnh độ sáng LED dựa trên cường độ ánh sáng môi trường
    // Giả sử: giá trị cảm biến cao -> môi trường sáng -> đèn cần tối hơn (hoặc tắt nếu quá sáng)
    //         giá trị cảm biến thấp -> môi trường tối -> đèn cần sáng hơn
    
    // Đặt đèn ở chế độ màu trắng khi điều chỉnh tự động để đảm bảo ánh sáng trắng phù hợp cho học tập/làm việc
    myLEDs.setColorMode(MODE_WHITE); 
    
    // Đảm bảo giá trị analog từ cảm biến không nhỏ hơn 0 hoặc lớn hơn 4095
    // Và độ sáng LED không nhỏ hơn 0 hoặc lớn hơn 255
    int desiredLedBrightness = map(ambientLightValue, 0, 4095, 255, 30); // 30 là mức sáng tối thiểu để đèn không tắt hẳn

    // Giới hạn lại độ sáng để tránh các giá trị bất thường
    desiredLedBrightness = constrain(desiredLedBrightness, 0, 255);

    // Chỉ điều chỉnh nếu đèn đang bật hoặc cần bật
    if (myLEDs.isLightOn() || desiredLedBrightness > 0) {
        myLEDs.setBrightness(desiredLedBrightness);
        // Sau khi đặt độ sáng, cần gửi trạng thái mới của đèn lên MQTT
        sendLEDState(); 
    } else if (!myLEDs.isLightOn() && desiredLedBrightness == 0) {
        myLEDs.turnOff(); // Đảm bảo đèn tắt nếu độ sáng yêu cầu là 0
        sendLEDState();
    }

    lastLightSensorReadMillis = millis(); // Cập nhật thời gian đọc cuối cùng
  }

    // Khi nhận lệnh điều khiển thủ công qua MQTT (ví dụ: BRIGHTNESS:X), autoBrightnessMode sẽ bị tắt.
    // Nếu bạn muốn điều khiển tự động lại, gửi lệnh "AUTO_BRIGHTNESS_ON" đến topic đèn.
}
