# MPU-6050 Diagnostic & Clone Detector

- **File:** `mpu6050_scanner.ino`
- **Purpose:** 
  - Scans I2C bus for addresses `0x68` and `0x69`
  - Reads register `0x75` (`WHO_AM_I`) to determine if the sensor is an authentic InvenSense MPU-6050 (`0x68`) or a Chinese clone (`0x98` / `0x70`)
  - Wakes up the chip by clearing register `0x6B` (`PWR_MGMT_1`)
  - Reads raw accelerometer and gyro registers to confirm live silicon response

## How to Run
1. Open `mpu6050_scanner.ino` in Arduino IDE.
2. Select board: `ESP32 Dev Module`.
3. Set Serial Monitor baud: `115200`.
4. Upload to see the exact silicon chip ID.
