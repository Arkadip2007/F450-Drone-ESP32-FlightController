/*
 * MPU6050 Fake/Clone Comprehensive Diagnostic & Scanner for ESP32
 * 
 * ESP32 Default I2C Pins:
 * SDA -> GPIO 21
 * SCL -> GPIO 22
 * VCC -> 3.3V or 5V (GY-521 modules usually have a 3.3V regulator, 5V is standard)
 * GND -> GND
 */

#include <Wire.h>

// I2C pins for ESP32 (change here if using different pins)
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22

// MPU6050 Register addresses
#define MPU_REG_WHO_AM_I   0x75
#define MPU_REG_PWR_MGMT_1 0x6B
#define MPU_REG_ACCEL_XOUT 0x3B

void identifyChip(uint8_t whoami) {
  Serial.print("   -> Chip Analysis: ");
  switch (whoami) {
    case 0x68:
      Serial.println("Authentic / Standard InvenSense MPU-6050");
      break;
    case 0x70:
      Serial.println("MPU-6500 (Remarked/Clone sold as MPU-6050)");
      break;
    case 0x71:
      Serial.println("MPU-9250 (Remarked/Clone sold as MPU-6050)");
      break;
    case 0x72:
      Serial.println("Clone MPU-6050 (Common Chinese revision)");
      break;
    case 0x73:
      Serial.println("MPU-9255 clone");
      break;
    case 0x75:
      Serial.println("MPU-6515 clone");
      break;
    case 0x98:
      Serial.println("Counterfeit/Clone MPU-6050 (Very common 0x98 clone)");
      break;
    case 0x00:
    case 0xFF:
      Serial.println("ERROR: Bus reading failed (0x00/0xFF). Check wiring or pull-up resistors.");
      break;
    default:
      Serial.print("Unknown / Unlisted Clone Chip (WHO_AM_I = 0x");
      Serial.print(whoami, HEX);
      Serial.println(")");
      break;
  }
}

void diagnoseMPU(uint8_t addr) {
  Serial.println("--------------------------------------------------");
  Serial.print("Testing device at I2C Address: 0x");
  if (addr < 16) Serial.print("0");
  Serial.println(addr, HEX);

  // 1. Read WHO_AM_I
  Wire.beginTransmission(addr);
  Wire.write(MPU_REG_WHO_AM_I);
  uint8_t err = Wire.endTransmission(false);
  
  if (err != 0) {
    Serial.println("   [FAIL] Could not send WHO_AM_I request!");
    return;
  }

  Wire.requestFrom(addr, (uint8_t)1);
  if (Wire.available()) {
    uint8_t whoami = Wire.read();
    Serial.print("   WHO_AM_I Register (0x75) Value : 0x");
    if (whoami < 16) Serial.print("0");
    Serial.print(whoami, HEX);
    Serial.print(" (Decimal: ");
    Serial.print(whoami);
    Serial.println(")");
    identifyChip(whoami);
  } else {
    Serial.println("   [FAIL] No response from WHO_AM_I register.");
  }

  // 2. Read PWR_MGMT_1
  Wire.beginTransmission(addr);
  Wire.write(MPU_REG_PWR_MGMT_1);
  Wire.endTransmission(false);
  Wire.requestFrom(addr, (uint8_t)1);
  if (Wire.available()) {
    uint8_t pwr = Wire.read();
    Serial.print("   PWR_MGMT_1 Register (0x6B)     : 0x");
    if (pwr < 16) Serial.print("0");
    Serial.print(pwr, HEX);
    if (pwr & 0x40) {
      Serial.println(" (Sleep mode: ACTIVE)");
    } else {
      Serial.println(" (Sleep mode: AWAKE)");
    }
  }

  // 3. Wake up device
  Wire.beginTransmission(addr);
  Wire.write(MPU_REG_PWR_MGMT_1);
  Wire.write(0x00); // Clear sleep bit
  err = Wire.endTransmission();
  if (err == 0) {
    Serial.println("   [OK] Sent Wake-Up command (Cleared sleep mode)");
  } else {
    Serial.println("   [FAIL] Failed to send Wake-Up command");
  }
  delay(100);

  // 4. Test reading raw sensor data (14 bytes: Accel X/Y/Z, Temp, Gyro X/Y/Z)
  Wire.beginTransmission(addr);
  Wire.write(MPU_REG_ACCEL_XOUT);
  Wire.endTransmission(false);
  Wire.requestFrom(addr, (uint8_t)14);
  if (Wire.available() >= 14) {
    int16_t ax = (Wire.read() << 8) | Wire.read();
    int16_t ay = (Wire.read() << 8) | Wire.read();
    int16_t az = (Wire.read() << 8) | Wire.read();
    int16_t temp = (Wire.read() << 8) | Wire.read();
    int16_t gx = (Wire.read() << 8) | Wire.read();
    int16_t gy = (Wire.read() << 8) | Wire.read();
    int16_t gz = (Wire.read() << 8) | Wire.read();

    float tempC = (temp / 340.0) + 36.53;

    Serial.println("   [OK] Test Sensor Readings successful:");
    Serial.printf("        Accel -> X: %d | Y: %d | Z: %d\n", ax, ay, az);
    Serial.printf("        Gyro  -> X: %d | Y: %d | Z: %d\n", gx, gy, gz);
    Serial.printf("        Temp  -> %.1f C\n", tempC);
  } else {
    Serial.println("   [FAIL] Could not read live sensor data registers.");
  }
  Serial.println("--------------------------------------------------");
}

void scanI2CBus() {
  Serial.println("\n==================================================");
  Serial.println(">> Scanning I2C Bus (Addresses 0x01 to 0x7F)...");
  Serial.println("==================================================");

  uint8_t count = 0;
  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("[FOUND] I2C Device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");

      diagnoseMPU(address);
      count++;
    } else if (error == 4) {
      Serial.print("[ERROR] Unknown error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (count == 0) {
    Serial.println("\n[!] NO I2C devices found!");
    Serial.println("    Please verify wiring:");
    Serial.println("    - VCC -> 3.3V or 5V");
    Serial.println("    - GND -> GND");
    Serial.println("    - SDA -> GPIO 21");
    Serial.println("    - SCL -> GPIO 22");
  } else {
    Serial.printf("\n[SUCCESS] Found %d device(s) on the I2C bus.\n", count);
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  delay(1000);

  Serial.println("\n\n##################################################");
  Serial.println("#   ESP32 MPU6050 Clone / Fake Diagnostic Tool   #");
  Serial.println("##################################################");

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(100000); // 100 kHz standard speed

  scanI2CBus();
}

void loop() {
  Serial.println("\nPress or type anything in Serial Monitor and press Enter to scan again...");
  while (!Serial.available()) {
    delay(500);
  }
  while (Serial.available()) Serial.read(); // clear buffer
  scanI2CBus();
}
