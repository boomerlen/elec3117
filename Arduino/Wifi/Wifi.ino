#include <WiFi.h>
#include <stdio.h>
#include <stdlib.h>

#define WIFI_SSID "Marlin from Finding Nemo"
#define WIFI_PSSWD "madsiscool"

#define HOST "172.20.10.1"
#define PORT 8000

#define DEVICE_ID "test_unit_0"

#define LABEL_SOIL "soil_moisture"
#define LABEL_DEVICE_ID "device_id"

int counter;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PSSWD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }

  counter = 0;

  Serial.println("WiFi connection successful.");
}

char *create_post(int data, char *label) {
  // Construct payload
  char payload[100];
  int payload_len = sprintf(payload, "%s=%s&%s=%d", LABEL_DEVICE_ID, DEVICE_ID, label, data);

  // Construct POST request
  char post_stack[1024];

  int post_len = sprintf(post_stack, "POST /device/data_entry/ HTTP/1.1\nHost: 172.20.10.3\nContent-Type: application/x-www-form-urlencoded\nContent-Length: %d\n\n%s", payload_len, payload);

  // Move buffer to stack so we can return ptr to it
  char *post_heap = (char *) malloc(sizeof(char) * (post_len+1));
  
  return strcpy(post_heap, post_stack);
}

void readResponse(WiFiClient *client){
  unsigned long timeout = millis();
  while(client->available() == 0){
    if(millis() - timeout > 5000){
      Serial.println(">>> Client Timeout !");
      client->stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while(client->available()) {
    String line = client->readStringUntil('\r');
    Serial.print(line);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  // Connection has been established
  // I'm stealing from https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/wifi.html

  Serial.print("Connecting to client: ");
  IPAddress ip(172, 20, 10, 3);

  Serial.println(ip);

  WiFiClient client;

  // Attempt connection to server
  if (!client.connect(ip, PORT)) {
    delay(500);
    return;
  }

  Serial.println("Connection to client successful.");

  // Construct web packet
  char *post_data = create_post(counter, LABEL_SOIL);

  Serial.print("Constructed post request with device id: ");
  Serial.print(DEVICE_ID);
  Serial.print(" and data: ");
  Serial.println(counter);

  client.print(post_data);
  readResponse(&client);

  Serial.println("Freeing memory.");
  free(post_data);

  delay(2000);
  counter++;  
}
