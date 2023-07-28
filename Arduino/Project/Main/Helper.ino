// Helper.ino

#include "esp_err.h"

void check_err(esp_err_t code) {
  if (code == ESP_OK) {
    return;
  }

  char msg[64];
  sprintf("Operation failed with message: %s.", esp_err_to_name(code));
  Serial.println(msg);
}