#include <Wire.h>

#define SLAVE_ADDRESS 0x08  // I2C address of the ESP32S3

void setup() {
  Wire.begin();  // Initialize I2C as master
  Serial.begin(115200);
  delay(1000);  // Wait for serial monitor to open
}

void loop() {
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write("Hello, World!");  // Send data to the slave
  Wire.endTransmission();
  Serial.println("Sent: Hello, World!");
  delay(1000);  // Wait for a second before sending the next message
}
