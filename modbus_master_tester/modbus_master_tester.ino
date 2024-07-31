#include <Wire.h>
#include <ModbusMaster.h>
#include <HardwareSerial.h>
#include "src/MAX7314/MAX7314.hpp"

// I2C Comms
const int8_t kSDA = 32;
const int8_t kSCL = 33;

HardwareSerial uart2(2);

ModbusMaster node;

// New lib
MAX7314 expanderOne;
MAX7314 expanderTwo;

int de_pin = 13;
// Example data
// count = 0x02 : 1 3 0 0 0 2 196 11 0 137 251 63 0 0 0 0 0 0 0 0 23 52 13 128 160 33 251 63 0 0 0 0 255 255 63 179 100 72 8 64 48 56 8 64 48 8 6 0 36 165 8 128 16 34 251 63 1 0 0 0 102 20 0 0
// count = 0x04 : 1 3 0 0 0 4 68 9 0 33 251 63 0 0 0 0 0 0 0 0 152 33 251 63 68 0 0 0 0 0 0 0 255 255 63 179 100 72 8 64 48 56 8 64 48 8 6 0 36 165 8 128 16 34 251 63 1 0 0 0 86 85 0 0

void pre_transmission() {
  // Serial.println("PRE TRANS");
  digitalWrite(de_pin, HIGH);
  delay(10);
}

void post_transmission() {
  // Serial.println("POST TRANS");
  digitalWrite(de_pin, LOW);
  delay(10);
}

void setup() {

  Serial.begin(115200);
  Wire.begin(kSDA, kSCL);

  uart2.begin(19200, SERIAL_8E1, 16, 17);
  // uart2.begin(19200, SERIAL_8N1, 16, 17);

  // New lib
  expanderOne.init(&Wire, 0x20, NULL, 0);
  expanderOne.configurePins(0xFF00);
  expanderTwo.init(&Wire, 0x24, NULL, 0);
  expanderTwo.configurePins(0x0000);

  expanderTwo.setPinsLow(STATIC_PIN2);

  pinMode(de_pin, OUTPUT);

  node.begin(1, uart2);
  node.preTransmission(pre_transmission);
  node.postTransmission(post_transmission);
}

void loop() {
  uint8_t result = node.readInputRegisters(0, 2);
  Serial.print("RESULT: ");
  Serial.println(result, HEX);
  Serial.println();
  delay(1000);
}

void test_uart() {
  pre_transmission();
  uart2.write('a');
  delay(10);
  post_transmission();
  uart2.flush();
  delay(1000);
}

// uint16_t data[6];

// result = node.readHoldingRegisters(0, 0x04);
// if (result == node.ku8MBSuccess) {
//   Serial.println("GOT RESPONSE!");
//   for (int i = 0; i < 6; i++) {
//     data[i] = node.getResponseBuffer(i);
//     Serial.print(data[i]);
//     Serial.print(" ");
//   }
//   Serial.println();
// }
