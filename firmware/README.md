# Firmware Directory

This directory contains both the **precompiled custom binary** and the **complete source tree** of the patched ESP-FC flight controller.

## Directory Contents

- **`espfc-v0.2.1-custom-bin/`**: Contains the merged, precompiled binary `espfc_v0.2.1_esp32_custom_0x00.bin` which can be flashed directly to `0x0` using `esptool.py` without requiring PlatformIO or compilation.
- **`esp-fc-source/`**: Contains the complete source code of ESP-FC (v0.2.1) with the MPU-6050 clone fix applied in `lib/Espfc/src/Device/GyroMPU6050.h`.

## Quick Flashing Command

```bash
esptool.py --chip esp32 --port COM3 --baud 921600 write_flash -z 0x0 espfc-v0.2.1-custom-bin/espfc_v0.2.1_esp32_custom_0x00.bin
```
