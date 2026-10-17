/*
 * ==============================================================================
 * ESP32 Drone Multi-Sensor All-In-One Live Telemetry & System Diagnostic
 * ==============================================================================
 * Modules Supported:
 * 1. MPU-6050 Clone (I2C: 0x68, WHO_AM_I: 0x98) -> Gyro, Accel, Live Pitch & Roll
 * 2. GY-271 Compass (Honeywell HMC5883L, I2C: 0x1E) -> Live Compass Heading & Cardinal
 * 3. HW-611 Barometer (Bosch BMP280, I2C: 0x76) -> Temp, Pressure, Relative Altitude
 * 4. GPS-6M / NEO-6M (Hardware UART Serial2) -> Live GPS Satellites, Fix & Coordinates
 * 5. ESP32 Performance Monitor -> Loop Rate (Hz), Free Heap (RAM), CPU Freq
 * ==============================================================================
 * WIRING GUIDE:
 *
 * [I2C Bus - Parallel Connection to same 2 pins]
 *   ESP32 GPIO 21 (SDA) <---> MPU6050 SDA + GY-271 SDA + BMP280 SDA
 *   ESP32 GPIO 22 (SCL) <---> MPU6050 SCL + GY-271 SCL + BMP280 SCL
 *   ESP32 GND           <---> All Sensors GND
 *   ESP32 3.3V          <---> BMP280 VCC + GY-271 VCC (+ MPU6050 VCC)
 *
 * [GPS-6M Module]
 *   ESP32 GPIO 16 (RX2) <---> GPS Module TX
 *   ESP32 GPIO 17 (TX2) <---> GPS Module RX
 *   ESP32 GND           <---> GPS GND
 *   ESP32 3.3V or 5V    <---> GPS VCC
 * ==============================================================================
 */

#include <Wire.h>
#include <math.h>

// I2C Pins
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22

// GPS UART Pins
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17
#define GPS_BAUD 9600

// I2C Addresses
#define ADDR_MPU6050 0x68
#define ADDR_HMC5883 0x1E
#define ADDR_BMP280  0x76

// Hardware status flags
bool mpuReady = false;
bool magReady = false;
bool baroReady = false;

// -----------------------------------------------------------------------------
// BMP280 Calibration & Calculations
// -----------------------------------------------------------------------------
struct BMP280_Calib {
  uint16_t dig_T1;
  int16_t  dig_T2, dig_T3;
  uint16_t dig_P1;
  int16_t  dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
} bmpCalib;

int32_t t_fine;
float baselinePressure = 0.0;
float currentPressure = 0.0;
float currentTemp = 0.0;
float relativeAltitude = 0.0;

// -----------------------------------------------------------------------------
// Live Sensor Variables
// -----------------------------------------------------------------------------
float rollAngle = 0.0;
float pitchAngle = 0.0;
float gyroRateX = 0.0, gyroRateY = 0.0, gyroRateZ = 0.0;

float compassHeading = 0.0;
const char* compassDirection = "N";

// GPS Tracking Variables
long gpsSentencesCount = 0;
bool gpsFix = false;
int  gpsSats = 0;
char gpsTime[12] = "--:--:--";
char gpsLat[16] = "Searching...";
char gpsLon[16] = "Searching...";

// Performance Monitor
unsigned long lastDisplayTime = 0;
unsigned long loopCounter = 0;
unsigned long loopHz = 0;
unsigned long lastHzTime = 0;

// =============================================================================
// 1. MPU-6050 Driver
// =============================================================================
bool initMPU6050() {
  Wire.beginTransmission(ADDR_MPU6050);
  Wire.write(0x75); // WHO_AM_I
  if (Wire.endTransmission(false) != 0) return false;
  Wire.requestFrom(ADDR_MPU6050, (uint8_t)1);
  if (!Wire.available()) return false;
  uint8_t who = Wire.read();
  if (who != 0x68 && who != 0x72 && who != 0x98) return false;

  // Wake up MPU6050 (PWR_MGMT_1 = 0x00)
  Wire.beginTransmission(ADDR_MPU6050);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();

  // Gyro scale 500 deg/s (FS_SEL = 1)
  Wire.beginTransmission(ADDR_MPU6050);
  Wire.write(0x1B);
  Wire.write(0x08);
  Wire.endTransmission();

  // Accel scale +/- 4g (AFS_SEL = 1)
  Wire.beginTransmission(ADDR_MPU6050);
  Wire.write(0x1C);
  Wire.write(0x08);
  Wire.endTransmission();

  return true;
}

void readMPU6050() {
  if (!mpuReady) return;

  Wire.beginTransmission(ADDR_MPU6050);
  Wire.write(0x3B);
  if (Wire.endTransmission(false) != 0) return;
  Wire.requestFrom(ADDR_MPU6050, (uint8_t)14);
  if (Wire.available() < 14) return;

  int16_t ax = (Wire.read() << 8) | Wire.read();
  int16_t ay = (Wire.read() << 8) | Wire.read();
  int16_t az = (Wire.read() << 8) | Wire.read();
  Wire.read(); Wire.read(); // Skip temp
  int16_t gx = (Wire.read() << 8) | Wire.read();
  int16_t gy = (Wire.read() << 8) | Wire.read();
  int16_t gz = (Wire.read() << 8) | Wire.read();

  // Calculate Roll & Pitch from Accelerometer (degrees)
  float ax_g = (float)ax / 8192.0;
  float ay_g = (float)ay / 8192.0;
  float az_g = (float)az / 8192.0;

  rollAngle  = atan2(ay_g, az_g) * 180.0 / M_PI;
  pitchAngle = atan2(-ax_g, sqrt(ay_g * ay_g + az_g * az_g)) * 180.0 / M_PI;

  // Gyro rates in deg/sec
  gyroRateX = (float)gx / 65.5;
  gyroRateY = (float)gy / 65.5;
  gyroRateZ = (float)gz / 65.5;
}

// =============================================================================
// 2. HMC5883L Magnetometer Driver
// =============================================================================
bool initHMC5883() {
  Wire.beginTransmission(ADDR_HMC5883);
  Wire.write(0x0A);
  if (Wire.endTransmission(false) != 0) return false;
  Wire.requestFrom(ADDR_HMC5883, (uint8_t)3);
  if (Wire.available() < 3) return false;
  char idA = Wire.read(), idB = Wire.read(), idC = Wire.read();
  if (idA != 'H' || idB != '4' || idC != '3') return false;

  // Set 75Hz, 8-average (Config A)
  Wire.beginTransmission(ADDR_HMC5883);
  Wire.write(0x00);
  Wire.write(0x78);
  Wire.endTransmission();

  // Set gain 1090 LSB/Gauss (Config B)
  Wire.beginTransmission(ADDR_HMC5883);
  Wire.write(0x01);
  Wire.write(0x20);
  Wire.endTransmission();

  // Continuous measurement mode (Mode reg)
  Wire.beginTransmission(ADDR_HMC5883);
  Wire.write(0x02);
  Wire.write(0x00);
  Wire.endTransmission();

  return true;
}

const char* getCardinalDirection(float heading) {
  if (heading >= 337.5 || heading < 22.5)  return "N  [North]";
  if (heading >= 22.5  && heading < 67.5)  return "NE [North-East]";
  if (heading >= 67.5  && heading < 112.5) return "E  [East]";
  if (heading >= 112.5 && heading < 157.5) return "SE [South-East]";
  if (heading >= 157.5 && heading < 202.5) return "S  [South]";
  if (heading >= 202.5 && heading < 247.5) return "SW [South-West]";
  if (heading >= 247.5 && heading < 292.5) return "W  [West]";
  return "NW [North-West]";
}

void readHMC5883() {
  if (!magReady) return;

  Wire.beginTransmission(ADDR_HMC5883);
  Wire.write(0x03);
  if (Wire.endTransmission(false) != 0) return;
  Wire.requestFrom(ADDR_HMC5883, (uint8_t)6);
  if (Wire.available() < 6) return;

  int16_t mx = (Wire.read() << 8) | Wire.read();
  int16_t mz = (Wire.read() << 8) | Wire.read();
  int16_t my = (Wire.read() << 8) | Wire.read();

  if (mx != -4096 && my != -4096) {
    float heading = atan2((float)my, (float)mx) * 180.0 / M_PI;
    if (heading < 0) heading += 360.0;
    compassHeading = heading;
    compassDirection = getCardinalDirection(compassHeading);
  }
}

// =============================================================================
// 3. BMP280 Barometer Driver
// =============================================================================
bool initBMP280() {
  Wire.beginTransmission(ADDR_BMP280);
  Wire.write(0xD0);
  if (Wire.endTransmission(false) != 0) return false;
  Wire.requestFrom(ADDR_BMP280, (uint8_t)1);
  if (!Wire.available()) return false;
  uint8_t chipId = Wire.read();
  if (chipId != 0x58 && chipId != 0x60 && chipId != 0x56 && chipId != 0x57) return false;

  // Read Calibration parameters
  Wire.beginTransmission(ADDR_BMP280);
  Wire.write(0x88);
  if (Wire.endTransmission(false) != 0) return false;
  Wire.requestFrom(ADDR_BMP280, (uint8_t)24);
  if (Wire.available() < 24) return false;

  uint8_t b[24];
  for (int i = 0; i < 24; i++) b[i] = Wire.read();

  bmpCalib.dig_T1 = (b[1] << 8) | b[0];
  bmpCalib.dig_T2 = (b[3] << 8) | b[2];
  bmpCalib.dig_T3 = (b[5] << 8) | b[4];
  bmpCalib.dig_P1 = (b[7] << 8) | b[6];
  bmpCalib.dig_P2 = (b[9] << 8) | b[8];
  bmpCalib.dig_P3 = (b[11] << 8) | b[10];
  bmpCalib.dig_P4 = (b[13] << 8) | b[12];
  bmpCalib.dig_P5 = (b[15] << 8) | b[14];
  bmpCalib.dig_P6 = (b[17] << 8) | b[16];
  bmpCalib.dig_P7 = (b[19] << 8) | b[18];
  bmpCalib.dig_P8 = (b[21] << 8) | b[20];
  bmpCalib.dig_P9 = (b[23] << 8) | b[22];

  // Config: filter x4, standby 16ms
  Wire.beginTransmission(ADDR_BMP280);
  Wire.write(0xF5);
  Wire.write(0x10);
  Wire.endTransmission();

  // Ctrl: Normal mode, osrs_t x2, osrs_p x16
  Wire.beginTransmission(ADDR_BMP280);
  Wire.write(0xF4);
  Wire.write(0x57);
  Wire.endTransmission();

  delay(50);
  return true;
}

void readBMP280() {
  if (!baroReady) return;

  Wire.beginTransmission(ADDR_BMP280);
  Wire.write(0xF7);
  if (Wire.endTransmission(false) != 0) return;
  Wire.requestFrom(ADDR_BMP280, (uint8_t)6);
  if (Wire.available() < 6) return;

  uint32_t p_msb = Wire.read(), p_lsb = Wire.read(), p_xlsb = Wire.read();
  uint32_t t_msb = Wire.read(), t_lsb = Wire.read(), t_xlsb = Wire.read();

  int32_t adc_P = (p_msb << 12) | (p_lsb << 4) | (p_xlsb >> 4);
  int32_t adc_T = (t_msb << 12) | (t_lsb << 4) | (t_xlsb >> 4);

  // Compensate Temperature
  int32_t var1 = ((((adc_T >> 3) - ((int32_t)bmpCalib.dig_T1 << 1))) * ((int32_t)bmpCalib.dig_T2)) >> 11;
  int32_t var2 = (((((adc_T >> 4) - ((int32_t)bmpCalib.dig_T1)) * ((adc_T >> 4) - ((int32_t)bmpCalib.dig_T1))) >> 12) * ((int32_t)bmpCalib.dig_T3)) >> 14;
  t_fine = var1 + var2;
  currentTemp = (float)((t_fine * 5 + 128) >> 8) / 100.0;

  // Compensate Pressure
  int64_t v1 = ((int64_t)t_fine) - 128000;
  int64_t v2 = v1 * v1 * (int64_t)bmpCalib.dig_P6;
  v2 = v2 + ((v1 * (int64_t)bmpCalib.dig_P5) << 17);
  v2 = v2 + (((int64_t)bmpCalib.dig_P4) << 35);
  v1 = ((v1 * v1 * (int64_t)bmpCalib.dig_P3) >> 8) + ((v1 * (int64_t)bmpCalib.dig_P2) << 12);
  v1 = (((((int64_t)1) << 47) + v1)) * ((int64_t)bmpCalib.dig_P1) >> 33;

  if (v1 != 0) {
    int64_t p = 1048576 - adc_P;
    p = (((p << 31) - v2) * 3125) / v1;
    v1 = (((int64_t)bmpCalib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    v2 = (((int64_t)bmpCalib.dig_P8) * p) >> 19;
    p = ((p + v1 + v2) >> 8) + (((int64_t)bmpCalib.dig_P7) << 4);
    currentPressure = (float)p / 25600.0; // hPa

    // Relative Altitude (meters) relative to ground startup pressure
    if (baselinePressure > 100.0) {
      relativeAltitude = 44330.0 * (1.0 - pow(currentPressure / baselinePressure, 0.1903));
    }
  }
}

// =============================================================================
// 4. GPS NMEA Parser
// =============================================================================
void processGPS() {
  while (Serial2.available()) {
    char c = Serial2.read();
    static char sentence[100];
    static int idx = 0;

    if (c == '\n' || c == '\r') {
      if (idx > 6) {
        sentence[idx] = '\0';
        gpsSentencesCount++;

        // Parse $GPGGA or $GNGGA for Satellites & Fix
        if (strstr(sentence, "GGA") != NULL) {
          char *p = sentence;
          int field = 0;
          char *token = strtok(p, ",");
          while (token != NULL) {
            field++;
            if (field == 2 && strlen(token) >= 6) { // UTC Time
              snprintf(gpsTime, sizeof(gpsTime), "%c%c:%c%c:%c%c", token[0], token[1], token[2], token[3], token[4], token[5]);
            }
            if (field == 7) { // Fix quality (0=No fix, 1=GPS fix, 2=DGPS)
              gpsFix = (token[0] > '0');
            }
            if (field == 8) { // Number of satellites
              gpsSats = atoi(token);
            }
            token = strtok(NULL, ",");
          }
        }
      }
      idx = 0;
    } else {
      if (idx < 98) sentence[idx++] = c;
    }
  }
}

// =============================================================================
// Setup
// =============================================================================
void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  delay(1000);

  Serial.println("\n\n=================================================================");
  Serial.println("  ESP32 DRONE ALL-IN-ONE SENSOR LIVE TEST & LOAD MONITOR         ");
  Serial.println("=================================================================");

  // Initialize I2C Bus
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(400000); // Fast 400kHz I2C for flight controller speed

  // Initialize GPS UART2
  Serial2.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

  // 1. MPU6050
  mpuReady = initMPU6050();
  Serial.printf("[1] MPU-6050 (0x68 / 0x98 Clone) : %s\n", mpuReady ? "CONNECTED (OK)" : "DISCONNECTED (FAIL)");

  // 2. HMC5883L
  magReady = initHMC5883();
  Serial.printf("[2] GY-271 Compass (0x1E HMC5883): %s\n", magReady ? "CONNECTED (OK)" : "DISCONNECTED (FAIL)");

  // 3. BMP280
  baroReady = initBMP280();
  Serial.printf("[3] HW-611 Baro (0x76 BMP280)    : %s\n", baroReady ? "CONNECTED (OK)" : "DISCONNECTED (FAIL)");

  // Calibrate baseline pressure (average 10 readings)
  if (baroReady) {
    Serial.print("    -> Calibrating ground altitude baseline... ");
    float sum = 0.0;
    for (int i = 0; i < 10; i++) {
      readBMP280();
      sum += currentPressure;
      delay(30);
    }
    baselinePressure = sum / 10.0;
    Serial.printf("Ground: %.2f hPa\n", baselinePressure);
  }

  // 4. GPS Initial check
  Serial.println("[4] GPS-6M (GPIO 16/17 UART2)    : LISTENING ON SERIAL2");
  Serial.println("=================================================================");
  Serial.println("Live Telemetry starting in 2 seconds...");
  delay(2000);
}

// =============================================================================
// Main Loop
// =============================================================================
void loop() {
  // Read all sensor hardware
  readMPU6050();
  readHMC5883();
  readBMP280();
  processGPS();

  // Loop performance tracker
  loopCounter++;
  if (millis() - lastHzTime >= 1000) {
    loopHz = loopCounter;
    loopCounter = 0;
    lastHzTime = millis();
  }

  // Print formatted Telemetry every 250ms (4 times per second)
  if (millis() - lastDisplayTime >= 250) {
    lastDisplayTime = millis();

    // Print clean ANSI header separator
    Serial.println("\n----------------------- [ DRONE LIVE TELEMETRY ] -----------------------");

    // 1. MPU-6050 Tilting & Motion
    if (mpuReady) {
      Serial.printf("[MPU6050 TILT]  Roll: %6.1f deg  |  Pitch: %6.1f deg  |  GyroZ: %6.1f dps\n", 
                    rollAngle, pitchAngle, gyroRateZ);
    } else {
      Serial.println("[MPU6050 TILT]  Sensor offline");
    }

    // 2. GY-271 Compass Orientation
    if (magReady) {
      Serial.printf("[COMPASS DIR ]  Heading: %5.1f deg  |  Facing: %s\n", 
                    compassHeading, compassDirection);
    } else {
      Serial.println("[COMPASS DIR ]  Sensor offline");
    }

    // 3. HW-611 Barometer Altitude & Pressure
    if (baroReady) {
      Serial.printf("[BARO ALTI   ]  Rel Alt: %+5.2f m   |  Press: %7.2f hPa |  Temp: %.1f C\n", 
                    relativeAltitude, currentPressure, currentTemp);
    } else {
      Serial.println("[BARO ALTI   ]  Sensor offline");
    }

    // 4. GPS-6M Satellite & Location
    if (gpsFix) {
      Serial.printf("[GPS-6M      ]  FIX LOCKED [OK]  |  Sats: %2d  |  UTC: %s\n", 
                    gpsSats, gpsTime);
    } else {
      Serial.printf("[GPS-6M      ]  Searching Sats...|  Packets: %ld  (Check window/antenna)\n", 
                    gpsSentencesCount);
    }

    // 5. ESP32 Brain Load & Performance
    uint32_t freeHeap = ESP.getFreeHeap();
    uint32_t cpuFreq  = ESP.getCpuFreqMHz();
    Serial.printf("[ESP32 LOAD  ]  Loop Speed: %lu Hz | Free RAM: %u Bytes | CPU: %u MHz\n", 
                  loopHz, freeHeap, cpuFreq);
  }
}
