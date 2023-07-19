
#include <Wire.h>

#include "SparkFunCCS811.h" 

#define CCS811_ADDR 0x5B //Default I2C Address(91)


CCS811 mySensor(CCS811_ADDR);

void setup()
{
  Serial.begin(115200);

  Wire.begin(); //Inialize I2C Hardware

  if (mySensor.begin() == false)
  {
    Serial.print("CCS811 error");
    while (1);
  }
}

void loop()
{
  //Check to see if data is ready with .dataAvailable()
  if (mySensor.dataAvailable())
  {
    //If so, have the sensor read and calculate the results.
    //Get them later
    mySensor.readAlgorithmResults();

    Serial.print("CO2: ");
    //Returns calculated CO2 reading
    Serial.println(mySensor.getCO2());
    Serial.print("tVOC:");
    //Returns calculated TVOC reading
    Serial.println(mySensor.getTVOC());
  }

  delay(10); 
}
