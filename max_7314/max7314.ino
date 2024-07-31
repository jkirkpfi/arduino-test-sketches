#include <Wire.h>
#include "src/MAX7314/max7314.h"

// For controlling GPIO
MAX7314 expanderOne;
MAX7314 expanderTwo;

// Interrupt Handling
bool interrupt_queued = false;
bool britebox_start = false;
unsigned long interrupt_current = 0;
unsigned long interrupt_previous = 0;
unsigned long interrupt_interval = 250;

/*
  !!! KEEP SIMPLE !!!
  Only do simple variable assignment.
  Nothing else!
*/
void IRAM_ATTR interrupt_handler_1() {
  interrupt_queued = true;
}

void setup() {
  Serial.begin(115200);
  Serial.println("BOARD START");
  // Setup MAX7314 GPIO Expanders
  // Configure pins 0 - 7 as OUTPUTS, 8 - 15 as INPUTS
  Serial.println("EXPANDER ONE INIT");
  expanderOne.init(&Wire, EXPANDER_1, interrupt_handler_1, 14);
  Serial.println("EXPANDER ONE INPUTS");
  expanderOne.configureInputs((MAX7314_StaticPins)0xFF00);
  Serial.println("EXPANDER ONE OUTPUTS");
  expanderOne.configureOutputs((MAX7314_StaticPins)0x00FF);

  // Configure all pins as OUTPUTS
  Serial.println("EXPANDER TWO INIT");
  expanderTwo.init(&Wire, EXPANDER_2, NULL, 0);
  Serial.println("EXPANDER TWO OUTPUTS");
  expanderTwo.configureOutputs((MAX7314_StaticPins)0xFFFF);
}

void loop() {
  // put your main code here, to run repeatedly:
}
