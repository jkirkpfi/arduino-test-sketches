#include <Wire.h>
#include "tsys01.h"
TSYS01 tsys;

#define TSYS01_ADDR 0x77
#define TSYS01_RESET 0x1E
#define TSYS01_ADC_READ 0x00
#define TSYS01_ADC_TEMP_CONV 0x48
#define TSYS01_PROM_READ 0XA0

enum PinNumbers {
  I2C_SDA = 21,
  I2C_SCL = 22,
};

uint16_t C[8];
uint32_t D1;
float TEMP;
uint32_t adc;

void tsys_init() {

  Wire.beginTransmission(TSYS01_ADDR);
  Wire.write(TSYS01_RESET);
  Wire.endTransmission();

  delay(10);

  // Read calibration values
  for (uint8_t i = 0; i < 8; i++) {
    Wire.beginTransmission(TSYS01_ADDR);
    Wire.write(TSYS01_PROM_READ + i * 2);
    Wire.endTransmission();

    // received_bytes += Wire.requestFrom(TSYS01_ADDR, 2);
    // C[i] = (Wire.read() << 8) | Wire.read();

    Wire.requestFrom(TSYS01_ADDR, 2);
    if (Wire.available() > 0) {
      C[i] = Wire.read();
      C[i] = (C[i] << 8) | Wire.read();
    }
  }

  Serial.print("CALIB: ");
  for (int i = 0; i < 8; i++) {
    Serial.printf("%d ", C[i]);
  }

  Serial.println();
}

void tsys_read() {

  Wire.beginTransmission(TSYS01_ADDR);
  Wire.write(TSYS01_ADC_TEMP_CONV);
  Wire.endTransmission();

  delay(10);  // Max conversion time per datasheet

  Wire.beginTransmission(TSYS01_ADDR);
  Wire.write(TSYS01_ADC_READ);
  Wire.endTransmission();

  Wire.requestFrom(TSYS01_ADDR, 3);
  D1 = 0;
  D1 = Wire.read();
  D1 = (D1 << 8) | Wire.read();
  D1 = (D1 << 8) | Wire.read();

  Serial.printf("D1: %d\n", D1);

  // calculate();
}





void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);

  tsys_init();
}

void loop() {

  tsys_1.read();
  float temp = tsys.temperature();
  Serial.printf("TEMP C: %.2f\n", temp);
  delay(1000);
}
