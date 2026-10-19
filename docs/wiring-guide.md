# ESP32 Drone Complete Wiring & Hardware Connection Guide

This guide describes how to connect all sensors, receivers, and ESCs to the ESP32-WROOM-32 (38-pin DevKit).

---

## 1. Shared I2C Sensor Bus (GPIO 21 & GPIO 22)

All I2C sensors (MPU-6050, BMP280, HMC5883L) connect **in parallel** to the exact same two pins on the ESP32:

```
ESP32 GPIO 21 (SDA)  <=======>  MPU-6050 SDA  +  BMP280 SDA  +  HMC5883L SDA
ESP32 GPIO 22 (SCL)  <=======>  MPU-6050 SCL  +  BMP280 SCL  +  HMC5883L SCL
ESP32 3.3V (or 5V)   <=======>  VCC pins of all 3 sensors
ESP32 GND            <=======>  GND pins of all 3 sensors (COMMON GROUND)
```

> **Pull-up Resistors Note:** Most breakout boards already feature onboard 4.7kΩ or 10kΩ pull-up resistors on the I2C lines. No external pull-ups are required unless long jumper wires (>15 cm) are used.

![Hardware Assembly](images/author-hardware-assembly-hand-soldering.jpg)

> **Bench Assembly & Continuity Testing:** Point-to-point wiring and checking bus continuity with a digital multimeter.

---

## 2. RC Receiver (FlySky i-BUS)

The FlySky receiver provides a single-wire serial data stream over the i-BUS protocol:

| Receiver Pin | ESP32 Pin | Purpose |
| :--- | :--- | :--- |
| **i-BUS Signal (White/Yellow)** | **GPIO 16 (RX2)** | High-speed 115200 baud serial packet stream |
| **VCC (Red)** | **5V (from ESC BEC)** | Powers the receiver (FlySky receivers require 4.0V–6.5V) |
| **GND (Black)** | **ESP32 GND** | Common Ground |

---

## 3. Quad-X Motor / ESC Connections

The ESP32 outputs standard analog PWM pulses (1000 µs – 2000 µs) to the ESC signal wires:

| Motor Number | Drone Position | Rotation Direction | ESP32 GPIO Pin |
| :--- | :--- | :--- | :--- |
| **Motor 1** | Rear-Right | Counter-Clockwise (CCW) | **GPIO 27** |
| **Motor 2** | Front-Right | Clockwise (CW) | **GPIO 25** |
| **Motor 3** | Rear-Left | Clockwise (CW) | **GPIO 4** |
| **Motor 4** | Front-Left | Counter-Clockwise (CCW) | **GPIO 12** |

---

## 4. GPS Module (u-blox NEO-6M)

| GPS Pin | ESP32 Pin | Purpose |
| :--- | :--- | :--- |
| **GPS TX** | **GPIO 16 (or GPIO 34)** | Serial data stream to ESP32 (NMEA sentences at 9600 baud) |
| **GPS RX** | **GPIO 17** | Configuration commands from ESP32 |
| **GPS VCC** | **5V (from ESC BEC)** | GPS antenna power |
| **GPS GND** | **GND** | Common Ground |

---

## 5. Power Distribution & Safety

> ⚠️ **CRITICAL WARNING:**
> 1. Never power the motors from the ESP32's 3.3V or 5V pins!
> 2. The 3S LiPo battery must connect directly to the Power Distribution Board (PDB) or ESC battery leads.
> 3. Ensure a common ground wire connects the ESC ground rail, the ESP32 GND, and the receiver GND to prevent floating logic signals.
