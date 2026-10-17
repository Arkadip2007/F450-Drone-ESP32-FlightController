# All Sensors Live Telemetry & System Diagnostic

- **File:** `all_sensors_live.ino`
- **Supported Sensors:**
  1. MPU-6050 (Addr `0x68`, accepts clone ID `0x98`) -> Live pitch, roll, and gyro rate
  2. HMC5883L (Addr `0x1E`) -> Live compass heading (0-360°) and cardinal direction (N, NE, E, etc.)
  3. BMP280 (Addr `0x76`) -> Atmospheric pressure, temperature, and zero-referenced relative altitude in meters
  4. NEO-6M GPS (Hardware UART2: GPIO 16 RX, GPIO 17 TX) -> Live NMEA packet tracking
  5. ESP32 Performance -> Measures loop frequency (Hz), free heap (RAM), and CPU clock

## How to Run
1. Open `all_sensors_live.ino` in Arduino IDE.
2. Select board: `ESP32 Dev Module`.
3. Set baud rate in Serial Monitor: `115200`.
4. Upload and watch real-time multi-sensor telemetry.
