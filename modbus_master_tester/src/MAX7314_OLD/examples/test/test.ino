/* 
 * Copyright (c) 2021 Device Solutions. All Rights Reserved.
 *
 * This file provides the interface between charging and fuel gauge components
 * and the main application
 * 
 * This file may be distributed under the terms of the License Agreement
 * provided with this software.
 * 
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <max7314.h>
#include <Wire.h>

#define EXAMPLE_VERSION "MAX7314 Example v1.0.1\r\n"

// GPIO Defines
// Only INT_PIN is used with the dev kit
#define SCL_PIN 23
#define SDA_PIN 18
#define INT_PIN 14

#define SERIAL_BAUD 9600
#define ONE_SECOND 1000
#define PRINT_BUFFER_LEN 128

// Our GPIO Expander objects
GpioExpander expanderOne;
GpioExpander expanderTwo;

// A buffer to use for printing debug
char printBuffer[PRINT_BUFFER_LEN];

// A volatile flag to keep track of whether we have an interrupt
// I2C comms should probably not be done in the interrupt thread - this lets us move it to the main thread
volatile bool interruptQueued;

// Keep track of the previous input values we got
uint16_t prevInputs;

// A pin change interrupt callback function
// ESP32 documentation indicates that interrupt functions should be stored in internal RAM instead of flash (IRAM_ATTR)
void IRAM_ATTR expanderOneCallback() {
  interruptQueued = true;
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  Wire.begin(SDA_PIN, SCL_PIN);
  delay(ONE_SECOND);
  Serial.print(EXAMPLE_VERSION);
  Serial.print(GPIO_EXPANDER_VERSION);

  // Initialize the first GPIO Expander
  expanderOne.init(&Wire, EXPANDER_1, expanderOneCallback, INT_PIN);

  // We only have one GPIO expander dev kit, so don't initialize the second
  //expanderTwo.init(&Wire, EXPANDER_2, NULL, 0);

  // All pins are inputs by default. Set these to outputs.
  // These correspond to LED pins on the GPIO Expander dev kit
  expanderOne.configureOutputs((GpioExpanderStaticPin)(STATIC_PIN0 |   // D1 Red
                                                       STATIC_PIN5 |   // D2 Blue
                                                       STATIC_PIN9 |   // D4 Red
                                                       STATIC_PIN10 |  // D4 Green
                                                       STATIC_PIN7));  // D3 Green


  // Try different PWM values

  // 100% on
  expanderOne.setOutputPwmValue(PWM_PIN5, 0);
  expanderOne.setOutputPwmValue(PWM_PIN0, 0);
  expanderOne.setOutputPwmValue(PWM_PIN7, 0);
  expanderOne.setOutputPwmValue(PWM_PIN9, 0);
  expanderOne.setOutputPwmValue(PWM_PIN10, 0);
  delay(ONE_SECOND);

  // Set D4 Blue to a static output too
  expanderOne.configureOutputs(STATIC_PIN11);
  expanderOne.clearStaticOutputs(STATIC_PIN11);
  delay(ONE_SECOND);


  expanderOne.setStaticOutputs(STATIC_PIN11);
  // 75% on
  expanderOne.setOutputPwmValue(PWM_PIN5, 4);
  expanderOne.setOutputPwmValue(PWM_PIN0, 4);
  expanderOne.setOutputPwmValue(PWM_PIN7, 4);
  expanderOne.setOutputPwmValue(PWM_PIN9, 4);
  expanderOne.setOutputPwmValue(PWM_PIN10, 4);
  delay(ONE_SECOND);

  // 50% on
  expanderOne.setOutputPwmValue(PWM_PIN5, 8);
  expanderOne.setOutputPwmValue(PWM_PIN0, 8);
  expanderOne.setOutputPwmValue(PWM_PIN7, 8);
  expanderOne.setOutputPwmValue(PWM_PIN9, 8);
  expanderOne.setOutputPwmValue(PWM_PIN10, 8);
  delay(ONE_SECOND);


  // 25% on
  expanderOne.setOutputPwmValue(PWM_PIN5, 12);
  expanderOne.setOutputPwmValue(PWM_PIN0, 12);
  expanderOne.setOutputPwmValue(PWM_PIN7, 12);
  expanderOne.setOutputPwmValue(PWM_PIN9, 12);
  expanderOne.setOutputPwmValue(PWM_PIN10, 12);
  delay(ONE_SECOND);

  // LEDs off
  expanderOne.setOutputPwmValue(PWM_PIN5, 16);
  expanderOne.setOutputPwmValue(PWM_PIN0, 16);
  expanderOne.setOutputPwmValue(PWM_PIN7, 16);
  expanderOne.setOutputPwmValue(PWM_PIN9, 16);
  expanderOne.setOutputPwmValue(PWM_PIN10, 16);
  expanderOne.setOutputPwmValue(PWM_PIN11, 16);
  delay(ONE_SECOND);


  // Set Pin 12 to an output, toggle it, then set it back to an input
  expanderOne.configureOutputs(STATIC_PIN12);
  expanderOne.clearStaticOutputs(STATIC_PIN12);
  delay(ONE_SECOND);
  expanderOne.setStaticOutputs(STATIC_PIN12);
  delay(ONE_SECOND);
  expanderOne.configureInputs(STATIC_PIN12);

  // Get inputs
  // This clears the interrupt pin from the GPIO driver that may have triggered when we configured outputs
  uint16_t inputVals = expanderOne.readInputs();
  sprintf(printBuffer, "Inputs: %02x %02x\r\n", (inputVals >> 8), inputVals & 0xFF);
  Serial.print(printBuffer);

  // Capture our previous inputs so we don't get an invalid value the first time we get an interrupt
  prevInputs = inputVals;
}

void loop() {
  // If we have an interrupt, print the input values
  if (interruptQueued) {
    interruptQueued = false;
    uint16_t inputVals = expanderOne.readInputs();
    sprintf(printBuffer, "Inputs changed.  Pin values: %02x %02x\r\n", (inputVals >> 8), inputVals & 0xFF);
    Serial.print(printBuffer);


    // See which pins have changed
    uint16_t changedPins = prevInputs ^ inputVals;
    if (changedPins & STATIC_PIN12) {
      Serial.print("Pin 12 changed\r\n");
    }
    if (changedPins & STATIC_PIN13) {
      Serial.print("Pin 13 changed\r\n");
    }
    if (changedPins & STATIC_PIN14) {
      Serial.print("Pin 14 changed\r\n");
    }
    if (changedPins & STATIC_PIN15) {
      Serial.print("Pin 15 changed\r\n");
    }

    // Save the new pin values
    prevInputs = inputVals;
  }
}
