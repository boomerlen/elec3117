// Sweeps register space for an I2C device.
#include <Wire.h>

#define I2C_ADDR 0x44

void setup() {
  // put your setup code here, to run once:
  Serial.being(115200);
  Wire.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
