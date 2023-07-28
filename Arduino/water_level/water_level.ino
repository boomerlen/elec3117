#include <Wire.h>

unsigned char low_data[8] = {0};
unsigned char high_data[12] = {0};

#define NO_TOUCH 0xFE
#define THRESHOLD 100
#define ATTINY1_HIGH_ADDR 0x78
#define ATTINY2_LOW_ADDR 0x77

void getHigh12SectionValue(void)
{
  // Clear the high_data array before getting new values
  memset(high_data, 0, sizeof(high_data));

  // Request 12 bytes of data from the ATTINY1_HIGH_ADDR
  Wire.requestFrom(ATTINY1_HIGH_ADDR, 12);
  while (12 != Wire.available()); // Wait until all 12 bytes are available

  // Read and store the 12 bytes into the high_data array
  for (int i = 0; i < 12; i++)
  {
    high_data[i] = Wire.read();
  }
  delay(10);
}

void getLow8SectionValue(void)
{
  // Clear the low_data array before getting new values
  memset(low_data, 0, sizeof(low_data));

  // Request 8 bytes of data from the ATTINY2_LOW_ADDR
  Wire.requestFrom(ATTINY2_LOW_ADDR, 8);
  while (8 != Wire.available()); // Wait until all 8 bytes are available

  // Read and store the 8 bytes into the low_data array
  for (int i = 0; i < 8; i++)
  {
    low_data[i] = Wire.read(); // receive a byte as char
  }
  delay(10);
}

void check()
{
  // Define the minimum and maximum sensor values for touch detection
  int sensorvalue_min = 250;
  int sensorvalue_max = 255;
  int low_count = 0;
  int high_count = 0;

  while (1)
  {
    uint32_t touch_val = 0;
    uint8_t trig_section = 0;
    low_count = 0;
    high_count = 0;

    // Read and display the values from the low_data array
    getLow8SectionValue();
    Serial.println("low 8 sections value = ");
    for (int i = 0; i < 8; i++)
    {
      Serial.print(low_data[i]);
      Serial.print(".");
      if (low_data[i] >= sensorvalue_min && low_data[i])
      {
        low_count++;
      }
    }
    Serial.println(" ");
    Serial.println(" ");

    // Read and display the values from the high_data array
    getHigh12SectionValue();
    Serial.println("high 12 sections value = ");
    for (int i = 0; i < 12; i++)
    {
      Serial.print(high_data[i]);
      Serial.print(".");
      if (high_data[i] >= sensorvalue_min && high_data[i])
      {
        high_count++;
      }
    }

    // Calculate touch_val based on the sensor values
    Serial.println(" ");
    Serial.println(" ");
    for (int i = 0; i < 8; i++)
    {
      if (low_data[i] > THRESHOLD)
      {
        touch_val |= 1 << i;
      }
    }

    for (int i = 0; i < 12; i++)
    {
      if (high_data[i] > THRESHOLD)
      {
        touch_val |= (uint32_t)1 << (8 + i);
      }
    }

    // Determine the triggered section (where touch is detected)
    while (touch_val & 0x01)
    {
      trig_section++;
      touch_val >>= 1;
    }

    // Print the water level percentage based on the triggered section
    Serial.print("water level = ");
    Serial.print(trig_section * 5);
    Serial.println("% ");
    Serial.println(" ");
    Serial.println("****");

    delay(1000);
  }
}

void setup()
{
  Serial.begin(115200);
  Wire.begin();
}

void loop()
{
  check();
}
