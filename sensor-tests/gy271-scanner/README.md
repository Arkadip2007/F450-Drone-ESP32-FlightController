# GY-271 Magnetometer / Compass Diagnostic

- **File:** `gy271_scanner.ino`
- **Purpose:**
  - Auto-identifies which compass chip is mounted on the breakout board:
    - **Authentic Honeywell HMC5883L:** I2C `0x1E`, Identification registers `0x0A`-`0x0C` return `'H'`, `'4'`, `'3'`
    - **QST QMC5883L:** I2C `0x0D`
    - **QMC5883P:** I2C `0x2C`
  - Initializes the compass in continuous measurement mode
  - Computes trigonometric azimuth heading ($0^\circ - 360^\circ$)

## How to Run
1. Open `gy271_scanner.ino` in Arduino IDE.
2. Select board: `ESP32 Dev Module`.
3. Set Serial Monitor baud: `115200`.
4. Upload and rotate sensor horizontally to observe heading changes.
