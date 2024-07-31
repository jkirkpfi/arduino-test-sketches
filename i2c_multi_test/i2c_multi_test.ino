
#include <Wire.h>
#include "TCA9548A.h"
// #include "TSYS01.h"
#include "tsys01_2.h"


#define MUX_ADDRESS 0x70

#define TSYS01_ADDR 0x77
#define TSYS01_RESET 0x1E
#define TSYS01_ADC_READ 0x00
#define TSYS01_ADC_TEMP_CONV 0x48
#define TSYS01_PROM_READ 0XA0

TCA9548A i2c_mux;
TSYS01 tsys_1;
TSYS01 tsys_2;

byte channels;

enum PinNumbers {
  I2C_SDA = 21,
  I2C_SCL = 22,
  UART_0_RX = 3,
  UART_0_TX = 1,
  UART_1_RX = 16,
  UART_1_TX = 17,
  UART_2_RX = 15,
  UART_2_TX = 2,
  INTERRUPT_PIN = 14,
};

// libs
void select_channel(int channel) {
  Wire.beginTransmission(MUX_ADDRESS);
  Wire.write(1 << channel);
  Wire.endTransmission();
}

void tca_write(uint8_t data) {
  Wire.beginTransmission(MUX_ADDRESS);
  Wire.write(data);
  Wire.endTransmission(true);
}

void open_channel(byte channel) {
  // uint8_t buff = 0x00;
  // buff = 1 << channel;
  // channels |= buff;
  channels |= (1 << channel);

  Wire.beginTransmission(MUX_ADDRESS);
  Wire.write(channels);
  Wire.endTransmission(true);
}

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);

  // i2c_mux.begin(Wire);
  // i2c_mux.closeAll();

  // i2c_mux.openChannel(0);
  // select_channel(0);
  // tsys_1.init();

  if (!tsys_1.init()) {
    Serial.println("TSYS01 not found!");
  }
  
  Wire.beginTransmission(TSYS01_ADDR);
  Wire.write(TSYS01_RESET);
  Wire.endTransmission();

  delay(10);
  int received_bytes = 0;
  // Read calibration values
  for (uint8_t i = 0; i < 8; i++) {
    Wire.beginTransmission(TSYS01_ADDR);
    Wire.write(TSYS01_PROM_READ + i * 2);
    Wire.endTransmission();

    received_bytes += Wire.requestFrom(TSYS01_ADDR, 2);
    C[i] = (Wire.read() << 8) | Wire.read();
  }

  // i2c_mux.openChannel(1);
  // select_channel(1);
  // tsys_2.init();
}

void loop() {

  // i2c_mux.openChannel(0);
  // open_channel(0);
  // select_channel(0);
  tsys_1.read();

  // i2c_mux.openChannel(1);
  // open_channel(1);
  // select_channel(1);
  // tsys_2.read();

  float temp_1 = tsys_1.temperature();
  // float temp_2 = tsys_2.temperature();

  Serial.printf("T: %.2f\n", temp_1);
  // Serial.printf("T: %.2f  %.2f\n", temp_1, temp_2);

  delay(1000);
}
