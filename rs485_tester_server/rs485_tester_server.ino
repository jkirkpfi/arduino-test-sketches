// Server Tester

#include <HardwareSerial.h>
HardwareSerial uart2(2);

int de_pin = 5;
int led_pin = 2;
int counter = 0;

void setup() {

  Serial.begin(115200);
  Serial.println("RS485 SERVER TESTER");
  uart2.begin(19200, SERIAL_8N1, 16, 17);

  pinMode(de_pin, OUTPUT);
  receive_mode();

  pinMode(led_pin, OUTPUT);
}

void loop() {

  if (uart2.available()) {
    char input = uart2.read();
    Serial.println(input);
    send_mode();
    uart2.write('b');
    delay(10);
    receive_mode();
  }

}
