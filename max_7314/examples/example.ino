#include <Wire.h>

// Define the I2C address of the MAX7314 GPIO Expander
const int max7314Address = 0x4C;  // Change this if the address is different

// Define the input port register address of the MAX7314
const byte inputPortRegister = 0x00;

// Define the number of sensors connected to the MAX7314
const int numSensors = 8;

// Array to store the sensor states
bool sensorStates[numSensors];

// Define the interval for reading the sensors in milliseconds
const unsigned long sensorReadInterval = 1000;  // Read every 1 second

// Variable to store the last time the sensors were read
unsigned long lastSensorReadTime = 0;

void setup() {
  Wire.begin();          // Initialize the I2C bus
  Serial.begin(115200);  // Initialize Serial communication for debugging
}

void loop() {
  // Check if it's time to read the sensors
  if (millis() - lastSensorReadTime >= sensorReadInterval) {
    readSensors();                  // Read the sensors
    printSensorStates();            // Print the sensor states (for debugging)
    lastSensorReadTime = millis();  // Update the last read time
  }

  // Your other non-blocking tasks can go here...
}

void readSensors() {
  Wire.beginTransmission(max7314Address);
  Wire.write(inputPortRegister);
  Wire.endTransmission();

  Wire.requestFrom(max7314Address, 1);
  byte inputPortData = Wire.read();

  for (int i = 0; i < numSensors; i++) {
    sensorStates[i] = bitRead(inputPortData, i);
  }
}

void printSensorStates() {
  Serial.print("Sensor States: ");
  for (int i = 0; i < numSensors; i++) {
    Serial.print(sensorStates[i]);
    Serial.print(" ");
  }
  Serial.println();
}