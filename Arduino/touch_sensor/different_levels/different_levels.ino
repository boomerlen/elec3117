const int touchPin = T0;  // GPIO pin connected to the touch sensor
const int lowThreshold = 5;   // Threshold for low water level (less than 5 cm)
const int mediumThreshold = 10;   // Threshold for medium water level (between 5 cm and 10 cm)
const int highThreshold = 20;   // Threshold for high water level (between 10 cm and 20 cm)

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for the Serial connection to be established
  }
  Serial.println("ESP32 Water Level Test");
  Serial.println("Water Level (cm)");

  // Enable Serial Plotter in the Arduino IDE
  Serial.println("#OpenPlotter");
}

void loop() {
  int touchValue = touchRead(touchPin);

  if (touchValue < lowThreshold) {
    Serial.println(touchValue);
    // Do something when low water level is detected
  }
  else if (touchValue >= lowThreshold && touchValue < mediumThreshold) {
    Serial.println(touchValue);
    // Do something when medium water level is detected
  }
  else if (touchValue >= mediumThreshold && touchValue < highThreshold) {
    Serial.println(touchValue);
    // Do something when high water level is detected
  }
  else {
    Serial.println(touchValue);
    // Do something when water level is above 20 cm
  }

  delay(100);
}
