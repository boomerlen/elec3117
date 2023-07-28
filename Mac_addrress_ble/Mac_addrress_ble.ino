#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_gap_bt_api.h>

void setup() {
  Serial.begin(115200);

  // Initialize the Bluetooth stack
  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  esp_bt_controller_init(&bt_cfg);
  esp_bt_controller_enable(ESP_BT_MODE_BTDM);

  // Print the MAC address
  uint8_t btAddress[6];
  esp_read_mac(btAddress, ESP_MAC_BT);
  Serial.print("Bluetooth MAC address: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(String(btAddress[i], HEX));
    if (i < 5) Serial.print(":");
  }
  Serial.println();

  // Disable the Bluetooth stack after getting the MAC address
  esp_bt_controller_disable(ESP_BT_MODE_BTDM);
}
void loop() {
  // Do nothing in the loop
}
