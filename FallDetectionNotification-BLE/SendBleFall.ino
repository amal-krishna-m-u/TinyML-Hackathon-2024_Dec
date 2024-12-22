#include "LSM6DS3.h"
#include "Wire.h"
#include <ArduinoBLE.h>

// BLE Service and Characteristic UUIDs
BLEService fallDetectionService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEByteCharacteristic fallDetectionCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

// LSM6DS3 sensor setup
LSM6DS3 lsm6ds3(I2C_MODE, 0x6A);
uint16_t detectCount = 0;
bool fallDetected = false;

void setup() {
    Serial.begin(9600);
    while (!Serial);

    // Initialize BLE
    if (!BLE.begin()) {
        Serial.println("Failed to initialize BLE!");
        while (1);
    }

    // Set up BLE service and characteristic
    BLE.setLocalName("FallDetector");
    BLE.setAdvertisedService(fallDetectionService);
    fallDetectionService.addCharacteristic(fallDetectionCharacteristic);
    BLE.addService(fallDetectionService);
    fallDetectionCharacteristic.writeValue(0);
    BLE.advertise();

    Serial.println("BLE Fall Detector initialized");

    // Initialize accelerometer
    if (lsm6ds3.begin() != 0) {
        Serial.println("Device error");
    } else {
        Serial.println("Device OK!");
    }

    if (0 != config_free_fall_detect()) {
        Serial.println("Fail to configure!");
    } else {
        Serial.println("Success to Configure!");
    }
}

void loop() {
    // Listen for BLE connections
    BLEDevice central = BLE.central();
    
    if (central) {
        Serial.print("Connected to central: ");
        Serial.println(central.address());
        
        while (central.connected()) {
            uint8_t readDataByte = 0;
            // Read the wake-up source register
            lsm6ds3.readRegister(&readDataByte, LSM6DS3_ACC_GYRO_WAKE_UP_SRC);
            // Mask off the FF_IA bit for free-fall detection
            readDataByte &= 0x20;
            
            if (readDataByte) {
                detectCount++;
                fallDetected = true;
                
                // Send fall detection event over BLE
                fallDetectionCharacteristic.writeValue(1);
                
                Serial.print("Free fall detected! Count: ");
                Serial.println(detectCount);
                
                // Also send the accumulated count
                // Note: In a real application, you might want to create a separate characteristic for this
                
                delay(100000);  
                
                // Reset the fall detection flag
                fallDetectionCharacteristic.writeValue(0);
                fallDetected = false;
            }
            delay(500);
        }
        
        Serial.print("Disconnected from central: ");
        Serial.println(central.address());
    }
}

int config_free_fall_detect(void) {
    uint8_t error = 0;
    uint8_t dataToWrite = 0;
    
    dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_200Hz;
    dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_2g;
    dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_416Hz;
    
    error += lsm6ds3.writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, dataToWrite);
    error += lsm6ds3.writeRegister(LSM6DS3_ACC_GYRO_WAKE_UP_DUR, 0x00);
    error += lsm6ds3.writeRegister(LSM6DS3_ACC_GYRO_FREE_FALL, 0x33);
    error += lsm6ds3.writeRegister(LSM6DS3_ACC_GYRO_MD1_CFG, 0x10);
    error += lsm6ds3.writeRegister(LSM6DS3_ACC_GYRO_MD2_CFG, 0x10);
    error += lsm6ds3.writeRegister(LSM6DS3_ACC_GYRO_TAP_CFG1, 0x81);
    
    return error;
}