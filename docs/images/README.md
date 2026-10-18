# 📸 Visual Documentation & Configurator Screenshots (ESP32)

This directory hosts high-resolution schematics, wiring diagrams, and configurator verification screenshots for the **F450 ESP32 Flight Controller (esp-fc)** project.

---

## 🖼️ Gallery of Images & Visual References

### 1. [ESP32 I2C Sensor Bus Wiring Schematic](esp32-i2c-wiring-schematic.png)
- **File:** `esp32-i2c-wiring-schematic.png`
- **What is shown:**
  - Full breadboard and schematic wiring between ESP32 DevKit V1 and sensors.
  - Parallel I2C bus wiring on **GPIO 21 (SDA)** and **GPIO 22 (SCL)** shared across MPU-6050, BMP280, and HMC5883L/QMC5883L compass.
  - GPS UART connection to **GPIO 16 (RX2)** and **GPIO 17 (TX2)**.
  - Power rail distribution (3.3V sensor logic, 5V receiver and GPS power).

---

### 2. [Betaflight Configurator via ESP32 Wi-Fi MSP](esp32-betaflight-configurator.png)
- **File:** `esp32-betaflight-configurator.png`
- **Configurator View:** Configuration tab
- **What is shown:**
  - Standard Betaflight Configurator connected to the ESP32 wirelessly over Wi-Fi Access Point (`esp-fc-xxxx`) using the MSP (MultiWii Serial Protocol) network bridge.
  - Mixer preview, sensor orientations, and live telemetry updates without plugging in a USB cable.

---

### 3. [CLI Hardware Pin Assignment & Configuration](esp32-cli-pin-assignment.png)
- **File:** `esp32-cli-pin-assignment.png`
- **Configurator View:** CLI tab
- **What is shown:**
  - Verification of custom GPIO pin assignments (`pin` command in esp-fc CLI).
  - Motor timer PWM channels, serial baud rates, and sensor address mapping.

---

### 4. [ESP32 Wi-Fi Access Point Connection](esp32-wifi-ap-connection.png)
- **File:** `esp32-wifi-ap-connection.png`
- **What is shown:**
  - Network discovery and wireless pairing to the onboard ESP32 SoftAP hotspot for field tuning and telemetry streaming.

---

### 5. [Web-Based Firmware Flashing (ESPTool)](esp32-firmware-flasher.png)
- **File:** `esp32-firmware-flasher.png`
- **What is shown:**
  - Browser-based WebSerial / esptool flashing workflow for writing the bootloader, partition table, and compiled `esp-fc` binary directly to ESP32 Flash memory.

---

## 🔗 Cross-References
- [Main README](../../README.md)
- [Wiring Guide](../wiring-guide.md)
- [Flashing Guide](../flashing-guide.md)
- [Master Project Documentation](../master-project-documentation.md)
- [Engineering Q&A Troubleshooting Journey](../engineering-qa-troubleshooting.md)
