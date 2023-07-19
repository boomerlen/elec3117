#include <Wire.h>

#define FDC1004_ADDRESS 0x50

#define SDA_PIN 21  // Replace with your ESP32's SDA pin
#define SCL_PIN 22  // Replace with your ESP32's SCL pin

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  // Configure FDC1004
  Wire.beginTransmission(FDC1004_ADDRESS);
  Wire.write(0x0C); // CONFIG register
  Wire.write(0x00); // Default values (no averaging, internal reference, 100 kHz)
  Wire.endTransmission();

  Serial.println("FDC1004 sensor initialized.");
}

void loop() {
  float capacitance = readCapacitance(0); // Read capacitance from channel 0

  Serial.print("Capacitance: ");
  Serial.print(capacitance);
  Serial.println("pF");

  delay(1000);
}

float readCapacitance(uint8_t channel) {
  // Trigger measurement
  Wire.beginTransmission(FDC1004_ADDRESS);
  Wire.write(0x0C);
  Wire.write(1);
  Wire.write(1<<7);
  Wire.endTransmission();
  
  Wire.beginTransmission(FDC1004_ADDRESS);
  Wire.write(0);
  int result = Wire.endTransmission(false);


  // Read measurement
  Wire.requestFrom(FDC1004_ADDRESS, 2);
  uint8_t msb = Wire.read();
  uint8_t lsb = Wire.read();

  // Combine MSB and LSB to form measurement value
  uint16_t measurement = (msb << 8) | lsb;

  // Calculate capacitance based on measurement value
  // Adjust the calculation based on your sensor and calibration
  float capacitance = (float)measurement * 0.00027;
  

  return capacitance;

  //Convert capacitance to soil moisture 
}
