# BMP280 Barometer Diagnostic Scanner

- **File:** `bmp280_scanner.ino`
- **Purpose:**
  - Scans I2C bus at addresses `0x76` and `0x77`
  - Reads register `0xD0` (`CHIP_ID`) to verify authentic Bosch Sensortec BMP280 (`0x58`) vs BME280 (`0x60`)
  - Reads all 24 factory-trimmed calibration registers (`dig_T1`–`dig_T3`, `dig_P1`–`dig_P9`)
  - Calculates real-time compensated temperature and atmospheric pressure

## How to Run
1. Open `bmp280_scanner.ino` in Arduino IDE.
2. Select board: `ESP32 Dev Module`.
3. Set Serial Monitor baud: `115200`.
4. Upload to verify barometer operation.
