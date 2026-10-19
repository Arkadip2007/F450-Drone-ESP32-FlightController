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

### 6. [Handmade Green Perfboard FC — Top Sockets & Component Layout](esp32-green-perfboard-top-headers.jpg)
- **File:** `esp32-green-perfboard-top-headers.jpg`
- **What is shown:**
  - Double-sided green plated-through-hole perfboard with female pin header sockets for the 30-pin ESP32 DevKit.
  - Breakout pins for I2C sensors and power lines.
  - Zero-cost prototyping layout for DIY home construction.

---

### 7. [Handmade Green Perfboard FC — Perimeter Ground Bus Architecture](esp32-green-perfboard-perimeter-ground-bus.jpg)
- **File:** `esp32-green-perfboard-perimeter-ground-bus.jpg`
- **What is shown:**
  - Underside view showing continuous soldered double-layer copper ground rail running around the board perimeter.
  - Eliminates tangled insulated ground wires, providing low-impedance ground returns and effortless modular expansion for future sensors.

---

### 8. [Minimal Reliable Sensor Telemetry in Betaflight](esp32-betaflight-minimal-sensors-telemetry.jpg)
- **File:** `esp32-betaflight-minimal-sensors-telemetry.jpg`
- **What is shown:**
  - Assembled board in hand in front of Betaflight Configurator.
  - 3D Quad-X model responding in real time with exactly 2 core sensors active (Gyro & Accelerometer on MPU-6050) for Phase 1 flight stabilization without bus overhead.

---

### 9. [Complete F450 Bench Setup with Active 4-Motor Control](esp32-drone-complete-bench-motor-control.jpg)
- **File:** `esp32-drone-complete-bench-motor-control.jpg`
- **What is shown:**
  - Complete F450 drone on workbench with ESP32 flight controller connected to PC.
  - All 4 motors independently controllable via software, achieving the Phase 1 milestone and clearing the platform for maiden flight tests.

---

## 🔗 Cross-References
- [Main README](../../README.md)
- [Wiring Guide](../wiring-guide.md)
- [Flashing Guide](../flashing-guide.md)
- [Master Project Documentation](../master-project-documentation.md)
- [Engineering Q&A Troubleshooting Journey](../engineering-qa-troubleshooting.md)

