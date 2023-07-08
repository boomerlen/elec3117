#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  // Write 0x08 to register 0x10 on device with address 0x28
  Wire.beginTransmission(0x28);
  Wire.write(0x10);
  Wire.write(0x08);
  Wire.endTransmission();

  delay(5); // Add a small delay for stability
  Serial.println("got here");

  // Read 1 byte from device with address 0x28 at register 0x10
  Wire.requestFrom(0x28, 1);
  if (Wire.available()) {
    byte b = Wire.read();
    Serial.println(b);
  } else {
    Serial.println("No data received");
  }

  delay(1000); // Add a delay between each loop iteration
}
