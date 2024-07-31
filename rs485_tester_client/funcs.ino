
void send_mode() {
  digitalWrite(de_pin, HIGH);
  led_on();
}

void receive_mode() {
  digitalWrite(de_pin, LOW);
  led_off();
}

void led_on() {  
  // Serial.println("LED ON");
  digitalWrite(led_pin, HIGH);
}

void led_off() {  
  // Serial.println("LED OFF");
  digitalWrite(led_pin, LOW);
}

void blink() {
  led_on();
  delay(1000);
  led_off();
  delay(1000);
}

void blink_gpio(int gpio_pin) {
  digitalWrite(gpio_pin, HIGH);
  delay(1000);
  digitalWrite(gpio_pin, LOW);
  delay(1000);
}
