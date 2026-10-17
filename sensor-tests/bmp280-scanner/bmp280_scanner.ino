/*
 * HW-611 / BMP280 (Barometer / Altitude) Diagnostic & Scanner for ESP32
 * 
 * Supports:
 * - Bosch BMP280 (Addresses 0x76 or 0x77, Chip ID: 0x58)
 * - Bosch BME280 (Addresses 0x76 or 0x77, Chip ID: 0x60)
 * - Clone BMP280 variants (Chip ID: 0x56, 0x57, etc.)
 * 
 * ESP32 Connections:
 * SDA -> GPIO 21
 * SCL -> GPIO 22
 * VCC -> 3.3V (HW-611 works best with 3.3V)
 * GND -> GND
 */

#include <Wire.h>

#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22

#define BMP280_REG_CHIPID   0xD0
#define BMP280_REG_RESET    0xE0
#define BMP280_REG_STATUS   0xF3
#define BMP280_REG_CTRL_MEAS 0xF4
#define BMP280_REG_CONFIG   0xF5
#define BMP280_REG_PRESS    0xF7
#define BMP280_REG_TEMP     0xFA

void diagnoseBaro(uint8_t addr) {
  Serial.printf("\n[+] Diagnosing Barometer at Address: 0x%02X\n", addr);

  // 1. Read Chip ID from Register 0xD0
  Wire.beginTransmission(addr);
  Wire.write(BMP280_REG_CHIPID);
  if (Wire.endTransmission(false) != 0) {
    Serial.println("   [FAIL] Could not send Chip ID read request.");
    return;
  }

  Wire.requestFrom(addr, (uint8_t)1);
  if (!Wire.available()) {
    Serial.println("   [FAIL] No response from Chip ID register 0xD0.");
    return;
  }

  uint8_t chipId = Wire.read();
  Serial.printf("   CHIP_ID Register (0xD0): 0x%02X (Decimal: %d)\n", chipId, chipId);

  Serial.print("   -> Chip Analysis: ");
  switch (chipId) {
    case 0x58:
      Serial.println("Authentic Bosch Sensortec BMP280 (Original!)");
      break;
    case 0x60:
      Serial.println("Authentic Bosch Sensortec BME280 (Barometer + Humidity!)");
      break;
    case 0x56:
    case 0x57:
      Serial.println("Clone / Counterfeit BMP280 (Common clone variant)");
      break;
    case 0x55:
      Serial.println("BMP085 / BMP180");
      break;
    case 0x10:
      Serial.println("SPL06 Barometer");
      break;
    default:
      Serial.printf("Unknown Barometer / Clone ID: 0x%02X\n", chipId);
      break;
  }

  // 2. Soft Reset
  Wire.beginTransmission(addr);
  Wire.write(BMP280_REG_RESET);
  Wire.write(0xB6);
  Wire.endTransmission();
  delay(10);

  // 3. Configure Normal mode, oversampling x2 temp, x16 pressure
  Wire.beginTransmission(addr);
  Wire.write(BMP280_REG_CTRL_MEAS);
  Wire.write(0x57); // osrs_t=2, osrs_p=5, mode=3 (normal)
  Wire.endTransmission();

  Wire.beginTransmission(addr);
  Wire.write(BMP280_REG_CONFIG);
  Wire.write(0x10); // filter x16
  Wire.endTransmission();
  delay(50);

  // 4. Read Raw Pressure and Temperature (6 bytes)
  Wire.beginTransmission(addr);
  Wire.write(BMP280_REG_PRESS);
  if (Wire.endTransmission(false) == 0) {
    Wire.requestFrom(addr, (uint8_t)6);
    if (Wire.available() >= 6) {
      uint32_t press_msb = Wire.read();
      uint32_t press_lsb = Wire.read();
      uint32_t press_xlsb = Wire.read();
      uint32_t temp_msb = Wire.read();
      uint32_t temp_lsb = Wire.read();
      uint32_t temp_xlsb = Wire.read();

      uint32_t raw_press = (press_msb << 12) | (press_lsb << 4) | (press_xlsb >> 4);
      uint32_t raw_temp = (temp_msb << 12) | (temp_lsb << 4) | (temp_xlsb >> 4);

      Serial.println("   [OK] Live Barometer Communication Test:");
      Serial.printf("        Raw Temperature Data : %u\n", raw_temp);
      Serial.printf("        Raw Pressure Data    : %u\n", raw_press);
      if (raw_press != 0 && raw_press != 0xFFFFF) {
        Serial.println("   [SUCCESS] Sensor is responding and measuring pressure/altitude!");
      } else {
        Serial.println("   [WARNING] Sensor returned empty or saturated pressure data.");
      }
    }
  }
}

void scanAndDiagnose() {
  Serial.println("\n==================================================");
  Serial.println(">> Scanning I2C Bus for HW-611 / BMP280...");
  Serial.println("==================================================");

  uint8_t count = 0;
  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.printf("[FOUND] I2C Device found at address 0x%02X\n", address);
      count++;

      if (address == 0x76 || address == 0x77) {
        Serial.printf(" -> Standard address for BMP280 / BME280 (0x%02X)\n", address);
        diagnoseBaro(address);
      } else {
        Serial.printf(" -> Unexpected address: 0x%02X. Testing registers:\n", address);
        diagnoseBaro(address);
      }
      Serial.println("--------------------------------------------------");
    }
  }

  if (count == 0) {
    Serial.println("\n[!] NO I2C devices found!");
    Serial.println("    Please verify wiring:");
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
  Serial.println("#   ESP32 HW-611 / BMP280 Barometer Diagnostic   #");
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
