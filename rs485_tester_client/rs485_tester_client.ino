// Client Tester

#include <HardwareSerial.h>
HardwareSerial uart2(2);

int led_pin = 2;
int de_pin = 5;
int counter = 111;

void setup() {

  Serial.begin(115200);
  Serial.println("RS485 CLIENT TESTER");
  uart2.begin(9600, SERIAL_8N1, 16, 17);

  pinMode(de_pin, OUTPUT);
  receive_mode();

  pinMode(led_pin, OUTPUT);
}

void loop() {

  send_mode();
  delay(5);
  uart2.write('a');
  delay(5);
  receive_mode();
  delay(500);

  // send_mode();
  // uart2.write('b');
  // receive_mode();
  // delay(500);

}
