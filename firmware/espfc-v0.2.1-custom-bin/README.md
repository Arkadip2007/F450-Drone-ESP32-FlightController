# Precompiled Custom ESP-FC Firmware Binary

## File Details
- **Binary Name:** `espfc_v0.2.1_esp32_custom_0x00.bin`
- **Size:** 1,040,448 bytes (~1.0 MB)
- **Base Architecture:** ESP-FC v0.2.1 for ESP32-WROOM-32
- **Flash Offset:** `0x00000` (or `0x0`)
- **Partition Scheme:** `partitions_4M_nota.csv` (Merged image with bootloader + partitions + app)

## Why this custom binary?
Standard ESP-FC rejects clone MPU-6050 sensors whose `WHO_AM_I` register returns `0x98`. This binary has `0x98` whitelisted in `GyroMPU6050.h`, allowing clone chips to boot and operate normally.

## Flashing Instructions
```bash
esptool.py --chip esp32 --port COM3 --baud 921600 write_flash -z 0x0 espfc_v0.2.1_esp32_custom_0x00.bin
```
