// #include <DS18B20.h>
// DS18B20 ds(15);

#include <SoftwareSerial.h>

SoftwareSerial uart1(2, 3); // RX, TX

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  // Serial.println(ds.getTempC());
  printf("TEMP: %f\n", ds.getTempC());
  delay(250);
}
