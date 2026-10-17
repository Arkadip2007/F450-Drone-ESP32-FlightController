# ESP32 Flight Controller Flashing & Build Guide

## Method 1: Flashing Precompiled Custom Binary (Recommended)

The precompiled binary `firmware/espfc-v0.2.1-custom-bin/espfc_v0.2.1_esp32_custom_0x00.bin` includes the bootloader, partition table, and application firmware in a single file.

### Prerequisites
1. Python installed with `esptool`:
   ```bash
   pip install esptool
   ```
2. ESP32 connected via USB cable (CP2102 or CH340 driver installed).

### Flashing Command
```bash
esptool.py --chip esp32 --port COM_PORT --baud 921600 write_flash -z 0x0 firmware/espfc-v0.2.1-custom-bin/espfc_v0.2.1_esp32_custom_0x00.bin
```
*(Replace `COM_PORT` with your port, e.g. `COM3` on Windows or `/dev/ttyUSB0` on Linux).*

---

## Method 2: Building from Source with PlatformIO

### Prerequisites
- [VS Code](https://code.visualstudio.com/)
- [PlatformIO IDE Extension](https://platformio.org/)

### Build & Upload Steps
1. In VS Code, choose **File > Open Folder...** and select:
   `firmware/esp-fc-source/esp-fc-0.2.1`
2. Open terminal in VS Code and run:
   ```bash
   pio run -e esp32
   ```
3. To upload to the board:
   ```bash
   pio run -e esp32 --target upload
   ```

---

## Method 3: Connecting to Betaflight Configurator

1. Open **Betaflight Configurator** (version 10.8 or newer).
2. Connect the ESP32 via USB and select the corresponding COM port (baud 115200).
3. Alternatively, connect via **Wi-Fi**:
   - The ESP32 creates an access point named `espfc` or joins your local network depending on configuration.
   - Select connection type `TCP` in Betaflight Configurator and enter the IP address (default `192.168.4.1`, port `23`).
4. Configure PID tuning, receiver channels, and flight modes graphically.
