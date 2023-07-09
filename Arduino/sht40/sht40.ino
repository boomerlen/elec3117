#include <Wire.h>

#define SHT40_ADDR 0x44  // I2C address of the SHT40 sensor

void setup() {
  Wire.begin();

  Serial.begin(115200);
  while (!Serial) {
    ;  // Wait for the serial connection to be established
  }

  Serial.println("SHT40 Temperature and Humidity Sensor Test");

  // Initialize the SHT40 sensor
  if (!initSHT40()) {
    Serial.println("Failed to initialize SHT40 sensor. Check wiring or power supply.");
    while (1);
  }
}

void loop() {
  // Read the temperature and humidity values from the SHT40 sensor
  float temperature, humidity;
  if (readSHT40Data(temperature, humidity)) {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C\tHumidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  } else {
    Serial.println("Failed to read data from SHT40 sensor.");
  }

  delay(1000);
}

bool initSHT40() {
  Wire.beginTransmission(SHT40_ADDR);
  Wire.write(0x24);  // Reset command
  Wire.endTransmission();

  delay(15);

  Wire.beginTransmission(SHT40_ADDR);
  Wire.write(0x35);  // Heater off command
  Wire.endTransmission();

  return true;
}

bool readSHT40Data(float& temperature, float& humidity) {
  Wire.beginTransmission(SHT40_ADDR);
  Wire.write(0x2C);  // Measurement command
  Wire.write(0x06);  // High repeatability, clock stretching enabled
  Wire.endTransmission();

  delay(15);

  Wire.requestFrom(SHT40_ADDR, 6);
  if (Wire.available() == 6) {
    byte msb1 = Wire.read();
    byte lsb1 = Wire.read();
    byte msb2 = Wire.read();
    byte lsb2 = Wire.read();
    byte crc1 = Wire.read();
    byte crc2 = Wire.read();

    uint16_t rawTemp = (msb1 << 8) | lsb1;
    uint16_t rawHumidity = (msb2 << 8) | lsb2;

    temperature = -45 + (175.0 * rawTemp / 65535.0);
    humidity = 100.0 * rawHumidity / 65535.0;

    return true;
  }

  return false;
}
