// I2C Sweeper

#include <Wire.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < 127; i++) {
    Wire.beginTransmission(i);
    if(!Wire.endTransmission()) {
      Serial.println("Ack received at:");
      Serial.println(i);
    }
  }

  delay(500);
}
