#include <Wire.h>
#include <HardwareSerial.h>
#include "src/MAX7314/MAX7314.hpp"

HardwareSerial uart2(2);

// I2C Comms
const int8_t kSDA = 32;
const int8_t kSCL = 33;

MAX7314 expanderOne;
MAX7314 expanderTwo;
int de_pin = 13;

enum MessageIndex {
  ID,
  FC,
  ADD_HIGH,
  ADD_LOW,
  COIL_HIGH,
  COIL_LOW,
  CRC_HIGH,
  CRC_LOW
};

uint8_t test_message[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0x44, 0x09 };
uint8_t message_length = 8;

const uint8_t buffer_length = 256;
uint8_t buffer[buffer_length];
uint8_t buffer_index = 0;

uint8_t server_id = 1;
uint8_t function_code = 3;
uint16_t register_address = 0;
uint16_t coil_count = 2;

void setup() {
  Wire.begin(kSDA, kSCL);
  Serial.begin(115200);
  uart2.begin(19200, SERIAL_8E1, 16, 17);

  pinMode(de_pin, OUTPUT);

  expanderOne.init(&Wire, 0x20, NULL, 0);
  expanderOne.configurePins(0xFF00);
  expanderTwo.init(&Wire, 0x24, NULL, 0);
  expanderTwo.configurePins(0x0000);

  expanderTwo.setPinsLow(STATIC_PIN2);

  buffer[ID] = server_id;
  buffer[FC] = function_code;
  buffer[ADD_HIGH] = highByte(register_address);
  buffer[ADD_LOW] = lowByte(register_address);
  buffer[COIL_HIGH] = highByte(coil_count);
  buffer[COIL_LOW] = lowByte(coil_count);

  uint16_t crc_val = get_crc(buffer, 6);
  buffer[CRC_HIGH] = highByte(crc_val);
  buffer[CRC_LOW] = lowByte(crc_val);

  for (int i = 0; i < buffer_length; i++) {
    Serial.print(buffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  uart2.flush();
}

void loop() {

  if (uart2.read() == -1) {
    Serial.println("READ = -1, FLUSH UART2");
  }

  Serial.println("SEND MESSAGE");
  send_message();

  Serial.println("READ MESSAGE");
  read_message();

  Serial.println("PRINT MESSAGE");
  print_message();
  delay(1000);

}

void pre_send() {
  digitalWrite(de_pin, HIGH);
}

void post_send() {
  digitalWrite(de_pin, LOW);
}

void send_message() {
  pre_send();
  for (int i = 0; i < message_length; i++) {
    uart2.write(buffer[i]);
  }
  uart2.flush();
  post_send();
}

void read_message() {
  buffer_index = 0;
  while (uart2.available() > 0) {
    int output = uart2.read();
    buffer[buffer_index] = output;
    Serial.print(buffer[buffer_index], HEX);
    Serial.print(" ");
    buffer_index++;
  }
  Serial.println();
}

void print_message() {
  for (int i = 0; i < buffer_length; i++) {
    Serial.print(buffer[i], HEX);
    Serial.print(" ");
  }
}

void build_message() {
  buffer[ID] = server_id;
  buffer[FC] = function_code;
  buffer[ADD_HIGH] = highByte(register_address);
  buffer[ADD_LOW] = lowByte(register_address);
  buffer[COIL_HIGH] = highByte(coil_count);
  buffer[COIL_LOW] = lowByte(coil_count);

  uint16_t crc_val = get_crc(buffer, 6);
  buffer[CRC_HIGH] = highByte(crc_val);
  buffer[CRC_LOW] = lowByte(crc_val);
}

static uint16_t crc16_update(uint16_t crc, uint8_t a) {
  int i;

  crc ^= a;
  for (i = 0; i < 8; ++i) {
    if (crc & 1)
      crc = (crc >> 1) ^ 0xA001;
    else
      crc = (crc >> 1);
  }
  return crc;
}

// Compute the MODBUS RTU CRC
uint16_t get_crc(uint8_t buffer[], int len) {
  for (int i = 0; i < len; i++) {
    Serial.print(buffer[i]);
    Serial.print(" ");
  }
  Serial.println();
  uint16_t crc = 0xFFFF;

  for (int pos = 0; pos < len; pos++) {
    crc ^= (uint16_t)buffer[pos];  // XOR byte into least sig. byte of crc

    for (int i = 8; i != 0; i--) {  // Loop over each bit
      if ((crc & 0x0001) != 0) {    // If the LSB is set
        crc >>= 1;                  // Shift right and XOR 0xA001
        crc ^= 0xA001;
      } else {      // Else LSB is not set
        crc >>= 1;  // Just shift right
      }
    }
  }
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
  return crc;
}

/*
  byte val1;
  while (uart2.write(buffer, sizeof(buffer)) == 8) {
    val1 = ModbusData();
    delay(5000);
  }

byte ModbusData() {
  byte i;
  pre_send();
  delay(10);

  if (uart2.write(buffer, sizeof(buffer)) == 8) {
    post_send();

    for (i = 0; i < buffer_length; i++) {
      //Serial.print(mod.read(),HEX);
      buffer_values[i] = uart2.read();
    }

    if (buffer_values[1] != 255 && buffer_values[2] != 255 && buffer_values[2] != 255) {
      Serial.println("Got:");
      for (int i = 0; i < buffer_length; i++) {
        Serial.print(buffer_values[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
  }

  return buffer_values[8];
}
*/
