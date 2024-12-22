#include <Wire.h>

#define SLAVE_ADDRESS 0x08  // I2C address of this device
#define BUFFER_SIZE 32      // Buffer size for incoming data

volatile bool dataReceived = false;
char receivedData[BUFFER_SIZE] = {0};

void receiveEvent(int howMany) {
  int index = 0;
  while (Wire.available() && index < BUFFER_SIZE - 1) {
    receivedData[index++] = Wire.read();  // Receive byte as a character
  }
  receivedData[index] = '\0';  // Null-terminate the string
  dataReceived = true;
}

void setup() {
  Serial.begin(115200);
  delay(1000);  // Wait for serial monitor to initialize
  Serial.println("Starting...");  // Indicate that the setup has started

  Wire.begin(SLAVE_ADDRESS);      // Initialize I2C as slave
  Wire.onReceive(receiveEvent);   // Register event handler
}

void loop() {
  if (dataReceived) {
    Serial.print("Received: ");
    Serial.println(receivedData);
    dataReceived = false;
  }
  // Other code can run here
}
