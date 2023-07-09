//ESP32 touch sensor

#include <stdio.h>
#include <string.h>
void setup() {
 Serial.begin(115200);
 delay(500);
 Serial.println("ESP32 Touch Test");
}

char msg[32];
int t0,t3; 
void loop() {
  t0=touchRead(T0); //GPIO4 
  delay(100);
  t3=touchRead(T3);
  sprintf (msg,"%d, %d",t0,t3);
  Serial.println(msg);
  delay(100);
  

}
