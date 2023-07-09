#include <Wire.h>
#include "Adafruit_CCS811.h"

Adafruit_CCS811 ccs;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;  // Wait for the serial connection to be established
  }

  Wire.begin();
  
  if (!ccs.begin(0x5B, &Wire)) {
    Serial.println("Failed to initialize CCS811 sensor. Check wiring or power supply.");
    while (1);
  }

  // Wait for the sensor to be ready
  while (!ccs.available());
  
  // Set the baseline value (optional)
  if (ccs.setBaseline()) {
    Serial.println("Baseline set successfully.");
  } else {
    Serial.println("Failed to set baseline.");
  }

  Serial.println("CCS811 Air Quality Sensor Test");
}

void loop() {
  if (ccs.available()) {
    if (!ccs.readData()) {
      Serial.print("CO2: ");
      Serial.print(ccs.geteCO2());
      Serial.print(" ppm\tTVOC: ");
      Serial.print(ccs.getTVOC());
      Serial.println(" ppb");
    } else {
      Serial.println("Failed to read data from CCS811 sensor.");
    }
  }

  delay(1000);
}
