String serial_buffer = "";

void serial_loop() {
  char c;
  while (Serial.available()) {
    c = Serial.read();
    if (c == '\n') {
      if (serial_buffer == "restart") {
        ESP.restart();
      } else if (serial_buffer == "reset_wifi") {
        EepromUpdate(22, 0);
        EEPROM.commit();
      }
      serial_buffer = "";
    } else if (c != '\r') {
      serial_buffer += c;
    }
  }
}
