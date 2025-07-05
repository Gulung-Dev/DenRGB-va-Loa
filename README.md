# Bàn Học Thông Minh ESP32 Điều Khiển Qua MQTT

Dự án này là một phần của hệ thống **bàn học thông minh** sử dụng ESP32 để điều khiển đèn LED RGB chiếu sáng bàn học và loa phát nhạc hỗ trợ việc học tập hoặc giải trí, tất cả thông qua giao thức MQTT. Hệ thống được thiết kế để kết nối với MQTT Broker `mqtt.flespi.io` và nhận lệnh từ các ứng dụng MQTT Client như MQTTX, cho phép tùy chỉnh không gian học tập

## Tính Năng

* **Điều khiển đèn LED RGB cho bàn học:**
    * Bật/Tắt đèn chiếu sáng bàn.
    * Điều chỉnh độ sáng (0-255) phù hợp với nhu cầu học tập hoặc đọc sách.
    * Chuyển chế độ màu giữa Trắng (cho học tập) và Vàng (cho thư giãn).
    * Báo cáo trạng thái hiện tại (bật/tắt, độ sáng, chế độ màu) về MQTT Broker.
* **Điều khiển loa hỗ trợ học tập/giải trí:**
    * Phát/Dừng một giai điệu mẫu (kiểu 8-bit/chiptune) có thể dùng làm nhạc nền thư giãn hoặc thông báo.
    * Điều chỉnh âm lượng (0-100) để không làm phiền môi trường xung quanh.
    * Báo cáo trạng thái hiện tại (đang phát/dừng, âm lượng) về MQTT Broker.
* **Kết nối mạng:** Sử dụng WiFi để kết nối internet và MQTT Broker, đảm bảo khả năng điều khiển từ xa.
* **Giao tiếp MQTT:** Gửi và nhận tin nhắn MQTT để điều khiển và báo cáo trạng thái một cách hiệu quả.

## Các Thành Phần Cần Thiết

### Phần Cứng

* **ESP32 Development Board:** 
* **Đèn LED RGB (RGB KY-009): để chiếu sáng bàn.
    * Các chân điều khiển (Red, Green, Blue)
* **Loa gốm:** để phát âm thanh, dùng chung với mạch khuyếch đại âm thanh LM386

## Cấu Trúc 

*`Example`
├── *`Example.ino`             # File chính (main loop, setup, MQTT handlers)\
├── * `LedRGBControl.h`        # Định nghĩa lớp điều khiển LED RGB\
├── * `LedRGBControl.cpp`      # Triển khai các hàm điều khiển LED RGB\
├── * `SpeakerControl.h`       # Định nghĩa lớp điều khiển Loa\
├── * `SpeakerControl.cpp`     # Triển khai các hàm điều khiển Loa\
└── * `README.md`              


