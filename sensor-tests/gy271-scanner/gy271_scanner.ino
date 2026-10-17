/*
 * GY-271 (Magnetometer / Compass) Diagnostic & Scanner for ESP32
 * 
 * Supports:
 * - Honeywell HMC5883L (Address 0x1E)
 * - QST QMC5883L (Address 0x0D)
 * - QST QMC5883P (Address 0x2C)
 * - Clones / Variants
 * 
 * ESP32 Connections:
 * SDA -> GPIO 21
 * SCL -> GPIO 22
 * VCC -> 3.3V (Recommended) or 5V
 * GND -> GND
 */

#include <Wire.h>

#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22

void testHMC5883L(uint8_t addr) {
  Serial.println("\n[+] Testing for HMC5883L registers...");
  
  // Read Identification Registers A, B, C (0x0A, 0x0B, 0x0C)
  Wire.beginTransmission(addr);
  Wire.write(0x0A);
  if (Wire.endTransmission(false) == 0) {
    Wire.requestFrom(addr, (uint8_t)3);
    if (Wire.available() >= 3) {
      char idA = Wire.read();
      char idB = Wire.read();
      char idC = Wire.read();
      Serial.printf("    ID Registers (0x0A-0x0C): '%c' '%c' '%c' (0x%02X 0x%02X 0x%02X)\n", 
                    idA, idB, idC, idA, idB, idC);
      if (idA == 'H' && idB == '4' && idC == '3') {
        Serial.println("    -> RESULT: Authentic Honeywell HMC5883L detected!");
      } else {
        Serial.println("    -> RESULT: Possible Clone or variant responding at 0x1E");
      }
    }
  }

  // Configure continuous mode (Register 0x02 = 0x00)
  Wire.beginTransmission(addr);
  Wire.write(0x02);
  Wire.write(0x00);
  Wire.endTransmission();
  delay(10);

  // Read Mag X, Z, Y (Registers 0x03 to 0x08)
  Wire.beginTransmission(addr);
  Wire.write(0x03);
  Wire.endTransmission(false);
  Wire.requestFrom(addr, (uint8_t)6);
  if (Wire.available() >= 6) {
    int16_t mx = (Wire.read() << 8) | Wire.read();
    int16_t mz = (Wire.read() << 8) | Wire.read();
    int16_t my = (Wire.read() << 8) | Wire.read();
    Serial.printf("    Live Mag Data -> X: %d | Y: %d | Z: %d\n", mx, my, mz);
  }
}

void testQMC5883L(uint8_t addr) {
  Serial.println("\n[+] Testing for QMC5883L registers...");

  // Read Chip ID register (0x0D)
  Wire.beginTransmission(addr);
  Wire.write(0x0D);
  if (Wire.endTransmission(false) == 0) {
    Wire.requestFrom(addr, (uint8_t)1);
    if (Wire.available()) {
      uint8_t chipId = Wire.read();
      Serial.printf("    CHIP ID Register (0x0D): 0x%02X (Decimal: %d)\n", chipId, chipId);
      if (chipId == 0xFF) {
        Serial.println("    -> RESULT: Standard QST QMC5883L (Very common on GY-271)");
      } else {
        Serial.printf("    -> RESULT: QMC clone variant (ID = 0x%02X)\n", chipId);
      }
    }
  }

  // Set reset/period (Register 0x0B = 0x01)
  Wire.beginTransmission(addr);
  Wire.write(0x0B);
  Wire.write(0x01);
  Wire.endTransmission();

  // Mode: 200Hz, 8G, 512 OSR, Continuous (Register 0x09 = 0x1D)
  Wire.beginTransmission(addr);
  Wire.write(0x09);
  Wire.write(0x1D);
  Wire.endTransmission();
  delay(15);

  // Read Mag X, Y, Z (Registers 0x00 to 0x05)
  Wire.beginTransmission(addr);
  Wire.write(0x00);
  Wire.endTransmission(false);
  Wire.requestFrom(addr, (uint8_t)6);
  if (Wire.available() >= 6) {
    int16_t mx = Wire.read() | (Wire.read() << 8);
    int16_t my = Wire.read() | (Wire.read() << 8);
    int16_t mz = Wire.read() | (Wire.read() << 8);
    Serial.printf("    Live Mag Data -> X: %d | Y: %d | Z: %d\n", mx, my, mz);
  }
}

void scanAndDiagnose() {
  Serial.println("\n==================================================");
  Serial.println(">> Scanning I2C Bus for GY-271 Magnetometer...");
  Serial.println("==================================================");

  uint8_t count = 0;
  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.printf("[FOUND] I2C Device found at address 0x%02X\n", address);
      count++;

      if (address == 0x1E) {
        Serial.println(" -> Typical address for Honeywell HMC5883L");
        testHMC5883L(address);
      } else if (address == 0x0D) {
        Serial.println(" -> Typical address for QST QMC5883L (Most common modern GY-271)");
        testQMC5883L(address);
      } else if (address == 0x2C) {
        Serial.println(" -> Typical address for QMC5883P");
        testQMC5883L(address);
      } else {
        Serial.println(" -> Unknown address for standard magnetometer. Testing registers:");
        testQMC5883L(address);
        testHMC5883L(address);
      }
      Serial.println("--------------------------------------------------");
    }
  }

  if (count == 0) {
    Serial.println("\n[!] NO I2C devices found!");
    Serial.println("    Please check wiring:");
    Serial.println("    VCC -> 3.3V | GND -> GND | SDA -> GPIO 21 | SCL -> GPIO 22");
  } else {
    Serial.printf("\n[SUCCESS] Scan complete. Found %d device(s).\n", count);
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  delay(1000);

  Serial.println("\n\n##################################################");
  Serial.println("#     ESP32 GY-271 Compass / Mag Diagnostic      #");
  Serial.println("##################################################");

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(100000);

  scanAndDiagnose();
}

void loop() {
  Serial.println("\nPress or type anything in Serial Monitor and press Enter to scan again...");
  while (!Serial.available()) {
    delay(500);
  }
  while (Serial.available()) Serial.read();
  scanAndDiagnose();
}
