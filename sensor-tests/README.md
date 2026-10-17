# Multi-Sensor Test & Diagnostic Suite

This directory contains standalone Arduino sketches designed to test, diagnose, and verify every sensor on the drone prior to running the full flight controller firmware.

## Key Design Principles
1. **Zero External Libraries:** All sketches use only the standard Arduino `<Wire.h>` library for I2C communication. No external library installations needed.
2. **Deep Register-Level Diagnosis:** Inspects internal silicon chip registers (`WHO_AM_I`, `CHIP_ID`, calibration coefficients) to identify authentic vs counterfeit sensors.
3. **Hardware Benchmarking:** Measures loop rate, free RAM, and bus stability under realistic load.

## Test Sketches Summary

| Sketch Directory | Description | Purpose |
| :--- | :--- | :--- |
| **`all-sensors-live/`** | Multi-sensor live telemetry monitor | Tests all 3 I2C sensors + GPS simultaneously, calculates pitch, roll, heading, altitude, and ESP32 loop speed. |
| **`mpu6050-scanner/`** | MPU-6050 diagnosis and clone detector | Scans I2C bus, checks register `0x75`, detects clone status (`0x98`), tests wake-up and raw readings. |
| **`bmp280-scanner/`** | BMP280 barometer diagnostic | Reads chip ID (`0x58`), verifies 24-byte factory calibration tables, reads real-time temperature and barometric pressure. |
| **`gy271-scanner/`** | GY-271 compass diagnostic | Differentiates between authentic Honeywell HMC5883L (`0x1E`), QMC5883L (`0x0D`), and QMC5883P (`0x2C`). Computes magnetic heading. |
