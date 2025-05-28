# ESP32-CYD Weather Station

*Một trạm thời tiết DIY dựa trên ESP32 và màn hình Cheap Yellow Display (CYD) để theo dõi các điều kiện môi trường tại nhà.*

[![last commit](https://img.shields.io/github/last-commit/Naammmdz/Weather-Station-ESP32-CYD?style=flat-square)](https://github.com/Naammmdz/Weather-Station-ESP32-CYD/commits/main)
[![top language](https://img.shields.io/github/languages/top/Naammmdz/Weather-Station-ESP32-CYD?style=flat-square)](https://github.com/Naammmdz/Weather-Station-ESP32-CYD)
[![languages](https://img.shields.io/github/languages/count/Naammmdz/Weather-Station-ESP32-CYD?style=flat-square)](https://github.com/Naammmdz/Weather-Station-ESP32-CYD)

**Built with:**

[![C](https://img.shields.io/badge/C-A8B9CC?style=for-the-badge&logo=c&logoColor=white)]()
[![ESP32](https://img.shields.io/badge/ESP32-E7352C?style=for-the-badge&logo=espressif&logoColor=white)]()
[![ESP-IDF](https://img.shields.io/badge/ESP--IDF-E7352C?style=for-the-badge&logo=espressif&logoColor=white)]() <!-- Hoặc PlatformIO / Arduino tùy theo bạn dùng -->
<!-- Thêm các cảm biến và thư viện cụ thể nếu có, ví dụ: -->
<!-- [![BME280](https://img.shields.io/badge/Sensor-BME280-blue?style=flat-square)]() -->
<!-- [![DHT22](https://img.shields.io/badge/Sensor-DHT22-orange?style=flat-square)]() -->
<!-- [![ILI9341](https://img.shields.io/badge/Display-ILI9341-green?style=flat-square)]() -->

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements & Setup](#software-requirements--setup)
  - [Development Environment](#development-environment)
  - [Libraries](#libraries)
  - [Configuration](#configuration)
- [Building and Flashing](#building-and-flashing)
- [Wiring Diagram](#wiring-diagram)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Contributing (Optional)](#contributing-optional)
- [Future Enhancements (Optional)](#future-enhancements-optional)

---

## Overview

Dự án này là một trạm quan trắc thời tiết sử dụng vi điều khiển ESP32 mạnh mẽ kết hợp với màn hình hiển thị màu (thường được gọi là "Cheap Yellow Display" - CYD, thường dùng chip ILI9341 hoặc tương tự) và các cảm biến môi trường. Mục tiêu là cung cấp thông tin thời tiết cục bộ như nhiệt độ, độ ẩm, áp suất không khí (và các thông số khác tùy thuộc vào cảm biến) một cách trực quan và dễ theo dõi.

Dữ liệu có thể được hiển thị trực tiếp trên màn hình CYD và/hoặc gửi lên một nền tảng IoT hoặc web server để theo dõi từ xa.

---

## Features

-   🌡️ **Đo nhiệt độ:** Hiển thị nhiệt độ hiện tại (độ C hoặc độ F).
-   💧 **Đo độ ẩm:** Hiển thị độ ẩm tương đối (%).
-   🌬️ **Đo áp suất không khí:** Hiển thị áp suất (hPa/kPa) (nếu sử dụng cảm biến như BME280/BMP280).
-   ☀️ **(Tùy chọn) Đo chỉ số UV, cường độ ánh sáng.**
-   🖥️ **Hiển thị trực quan:** Dữ liệu được hiển thị rõ ràng trên màn hình màu TFT LCD (CYD).
-   🌐 **(Tùy chọn) Kết nối Wi-Fi:**
    -   Đồng bộ thời gian qua NTP.
    -   Gửi dữ liệu lên các dịch vụ IoT (ThingSpeak, MQTT, Blynk, etc.).
    -   Hiển thị dữ liệu qua một web server nhỏ trên ESP32.
-   💾 **(Tùy chọn) Lưu trữ dữ liệu:** Lưu trữ lịch sử đo đạc vào thẻ SD hoặc bộ nhớ flash SPIFFS/LittleFS.
-   🔋 **(Tùy chọn) Chế độ tiết kiệm năng lượng:** Cho các ứng dụng chạy bằng pin.

---

## Hardware Requirements

-   **Vi điều khiển:**
    -   ESP32 Development Board (ví dụ: ESP32-DevKitC, ESP32-WROOM-32 module kèm board mạch tùy chỉnh).
    -   Hoặc một bo mạch tích hợp ESP32 và màn hình CYD (ví dụ: WT32-SC01, T-Display ESP32).
-   **Màn hình hiển thị:**
    -   Màn hình TFT LCD màu, thường là loại 2.4", 2.8" hoặc 3.2" với độ phân giải 320x240, sử dụng driver ILI9341 (hoặc ST7789V cho một số biến thể CYD). Thường có giao tiếp SPI và cảm ứng (tùy chọn).
-   **Cảm biến thời tiết:**
    -   **Nhiệt độ & Độ ẩm:** DHT11, DHT22 (AM2302), SHT3x, BME280, BMP280.
    -   **Áp suất:** BME280, BMP280.
    -   **(Tùy chọn) Cảm biến chất lượng không khí:** CCS811, BME680.
    -   **(Tùy chọn) Cảm biến ánh sáng:** BH1750, LDR.
-   **Kết nối:** Dây cắm breadboard (jumper wires), breadboard (nếu cần).
-   **Nguồn cung cấp:** 5V USB hoặc nguồn pin phù hợp.
-   **(Tùy chọn) Module thẻ nhớ SD card:** Nếu cần lưu trữ dữ liệu.
-   **(Tùy chọn) Module RTC (Real-Time Clock):** Ví dụ DS3231 (nếu cần thời gian chính xác khi không có Wi-Fi).

---

## Software Requirements & Setup

### Development Environment

Chọn một trong các môi trường phát triển sau:

1.  **ESP-IDF (Espressif IoT Development Framework):**
    -   Đây là framework chính thức từ Espressif, cung cấp nhiều tính năng và kiểm soát sâu hệ thống.
    -   Cài đặt theo hướng dẫn tại [Espressif ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/).
2.  **PlatformIO IDE:**
    -   Một môi trường phát triển đa nền tảng, hỗ trợ ESP32 với ESP-IDF hoặc Arduino Core.
    -   Thường được tích hợp vào VS Code.
    -   Cài đặt: [PlatformIO IDE](https://platformio.org/platformio-ide).
3.  **Arduino IDE với ESP32 Core:**
    -   Cách tiếp cận đơn giản hơn, phù hợp nếu bạn đã quen với Arduino.
    -   Cài đặt ESP32 Core: [Arduino ESP32 Core Installation](https://github.com/espressif/arduino-esp32#installation-instructions).

### Libraries

Tùy thuộc vào phần cứng và tính năng bạn chọn, bạn sẽ cần các thư viện sau (tên thư viện có thể khác nhau giữa ESP-IDF và Arduino):

-   **Driver màn hình:**
    -   Ví dụ: `TFT_eSPI` by Bodmer (cho Arduino/PlatformIO), hoặc các component LVGL/driver trực tiếp cho ESP-IDF.
    -   Thư viện cho chip điều khiển cảm ứng (nếu có, ví dụ XPT2046).
-   **Driver cảm biến:**
    -   Ví dụ: `Adafruit BME280 Library`, `DHT sensor library` by Adafruit, `Adafruit SHT31 Library` (cho Arduino/PlatformIO).
    -   Các component driver tương ứng cho ESP-IDF.
-   **Kết nối Wi-Fi:** Thư viện `WiFi.h` (Arduino) hoặc các API Wi-Fi của ESP-IDF.
-   **NTP Client:** Thư viện `NTPClient.h` (Arduino) hoặc API sntp của ESP-IDF.
-   **MQTT Client:** Ví dụ `PubSubClient.h` (Arduino).
-   **SPIFFS/LittleFS/SD Card:** Thư viện `FS.h`, `SPIFFS.h`, `SD.h` (Arduino) hoặc API VFS của ESP-IDF.
-   **(Tùy chọn) Thư viện đồ họa:** LVGL (Light and Versatile Graphics Library) nếu bạn muốn giao diện người dùng phức tạp hơn.

Hãy liệt kê các thư viện cụ thể bạn đã sử dụng trong dự án và cách cài đặt chúng (ví dụ: qua Arduino Library Manager, PlatformIO Library Manager, hoặc git submodule cho ESP-IDF).

### Configuration

Trong code, bạn cần cấu hình các thông số sau (thường trong một file `config.h` hoặc tương tự):

-   **Chân GPIO:** Định nghĩa các chân SPI cho màn hình (MOSI, MISO, SCK, CS, DC, RST, BL), chân I2C/SPI cho cảm biến.
-   **Thông tin Wi-Fi:** `WIFI_SSID` và `WIFI_PASSWORD` (nếu sử dụng).
-   **Thông tin MQTT Broker/ThingSpeak API Key:** (nếu sử dụng).
-   **Ngưỡng cảnh báo, đơn vị hiển thị, tần suất cập nhật.**
-   **Cấu hình màn hình:** Loại driver, kích thước, xoay màn hình.

---

## Building and Flashing

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/Naammmdz/Weather-Station-ESP32-CYD.git
    cd Weather-Station-ESP32-CYD
    ```

2.  **Mở dự án bằng IDE/Toolchain bạn đã chọn.**

3.  **Cài đặt các thư viện cần thiết** (như đã liệt kê ở mục Libraries).

4.  **Cấu hình dự án:**
    *   **ESP-IDF:** Chạy `idf.py menuconfig` để cấu hình các thông số dự án, bao gồm phân vùng bộ nhớ, các component, v.v.
    *   **PlatformIO:** Chỉnh sửa file `platformio.ini` để chọn board, framework, thư viện, upload port.
    *   **Arduino IDE:** Chọn đúng board ESP32 (ví dụ: "ESP32 Dev Module" hoặc bo mạch CYD cụ thể nếu có) và port COM.

5.  **Build/Compile dự án:**
    *   **ESP-IDF:** `idf.py build`
    *   **PlatformIO:** `pio run` hoặc nút Build trong VS Code.
    *   **Arduino IDE:** Nút Verify/Compile (dấu tick).

6.  **Flash/Upload firmware:**
    *   Kết nối ESP32 với máy tính qua USB.
    *   **ESP-IDF:** `idf.py -p /dev/ttyUSB0 flash` (thay `/dev/ttyUSB0` bằng port COM của bạn). Bạn có thể gộp build và flash: `idf.py -p /dev/ttyUSB0 build flash monitor`
    *   **PlatformIO:** `pio run -t upload` hoặc nút Upload trong VS Code.
    *   **Arduino IDE:** Nút Upload (mũi tên).
    *   *Lưu ý: Một số bo mạch ESP32 cần giữ nút BOOT/FLASH trong khi upload.*

7.  **Mở Serial Monitor** để xem log và debug (baudrate thường là 115200).

---

## Wiring Diagram

*Nên có một sơ đồ kết nối rõ ràng ở đây. Bạn có thể vẽ bằng Fritzing, draw.io, hoặc các công cụ khác.*

Ví dụ mô tả kết nối (thay thế bằng sơ đồ thực tế):

-   **Màn hình CYD (SPI):**
    -   VCC -> 3.3V ESP32
    -   GND -> GND ESP32
    -   CS   -> GPIO [số chân CS của bạn]
    -   RESET-> GPIO [số chân RST của bạn]
    -   DC/RS-> GPIO [số chân DC của bạn]
    -   SDI/MOSI -> GPIO [số chân MOSI của bạn, thường là GPIO23 cho VSPI]
    -   SCK  -> GPIO [số chân SCK của bạn, thường là GPIO18 cho VSPI]
    -   LED  -> GPIO [số chân BL của bạn] (hoặc nối trực tiếp 3.3V nếu không điều khiển được đèn nền)
    -   SDO/MISO -> GPIO [số chân MISO của bạn, thường là GPIO19 cho VSPI] (nếu màn hình có đọc dữ liệu)
-   **Cảm biến BME280 (I2C):**
    -   VCC -> 3.3V ESP32
    -   GND -> GND ESP32
    -   SCL -> GPIO [số chân SCL của bạn, thường là GPIO22]
    -   SDA -> GPIO [số chân SDA của bạn, thường là GPIO21]
-   **Cảm biến DHT22 (One-Wire):**
    -   VCC -> 3.3V ESP32
    -   GND -> GND ESP32
    -   DATA -> GPIO [số chân DATA của bạn] (cần điện trở kéo lên 4.7k-10k Ohm giữa VCC và DATA)

---

## Usage

Sau khi nạp firmware thành công và kết nối phần cứng đúng cách:

1.  Cấp nguồn cho ESP32.
2.  Màn hình CYD sẽ khởi động và bắt đầu hiển thị dữ liệu thời tiết.
3.  Nếu đã cấu hình Wi-Fi, thiết bị sẽ cố gắng kết nối và đồng bộ thời gian/gửi dữ liệu.
4.  Quan sát các thông số trên màn hình.
5.  (Tùy chọn) Truy cập web server (nếu có) qua địa chỉ IP của ESP32 để xem dữ liệu.

---

## Project Structure

Mô tả ngắn gọn về cấu trúc thư mục chính của dự án:

```
Weather-Station-ESP32-CYD/
├── main/                     # (Đối với ESP-IDF, chứa code chính)
│   ├── main.c                # File chính
│   ├── Kconfig               # Cấu hình component (nếu là component)
│   └── CMakeLists.txt        # Build script cho component
├── src/                      # (Đối với Arduino/PlatformIO, chứa code chính)
│   └── main.cpp              # File chính (hoặc .ino)
├── include/                  # (Đối với Arduino/PlatformIO, chứa các file header)
│   └── config.h              # (Ví dụ: file cấu hình)
├── lib/                      # (Đối với PlatformIO, chứa các thư viện cục bộ)
├── components/               # (Đối với ESP-IDF, chứa các component tùy chỉnh)
│   ├── sensor_driver/
│   └── display_controller/
├── data/                     # (Đối với PlatformIO/ESP-IDF, chứa file cho SPIFFS/LittleFS)
├── CMakeLists.txt            # (Đối với ESP-IDF, build script chính của dự án)
├── platformio.ini            # (Đối với PlatformIO, file cấu hình dự án)
└── README.md
```
*(Điều chỉnh cấu trúc này cho phù hợp với dự án thực tế của bạn)*

---

## Contributing (Optional)

Nếu bạn muốn người khác đóng góp vào dự án:

Mọi đóng góp đều được chào đón! Vui lòng fork repository và tạo một pull request với các thay đổi của bạn. Để biết thêm chi tiết, hãy xem `CONTRIBUTING.md` (nếu có).

---

## Future Enhancements (Optional)

Liệt kê các ý tưởng để phát triển thêm cho dự án:

-   Thêm nhiều loại cảm biến hơn (chất lượng không khí, gió, mưa).
-   Giao diện người dùng đồ họa (GUI) đẹp hơn trên màn hình CYD sử dụng LVGL.
-   Hỗ trợ màn hình cảm ứng để tương tác.
-   Lưu trữ dữ liệu dài hạn trên thẻ SD hoặc gửi lên database đám mây.
-   Thông báo cảnh báo thời tiết (qua email, notification).
-   Tích hợp trợ lý ảo.

---

⬆️ [Return to Top](#table-of-contents)
