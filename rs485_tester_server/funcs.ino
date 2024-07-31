
void send_mode() {
  digitalWrite(de_pin, HIGH);
  led_on();
}

void receive_mode() {
  digitalWrite(de_pin, LOW);
  led_off();
}

void led_on() {
  digitalWrite(led_pin, HIGH);
}

void led_off() {
  digitalWrite(led_pin, LOW);
}
