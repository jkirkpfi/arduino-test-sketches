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

#include "Arduino.h"
#include "max7314.h"
// #include <cstdio>
// using namespace std;

// The GPIO Expander auto-increments registers, so for any _REG_0 register, we don't need to define any others
#define INPUT_REG_0 0x00
#define OUTPUT_REG_0 0x02
#define PORT_CONFIG_REG_0 0x06
#define PWM_REG_0 0x10
#define PINS_PER_PWM_REGISTER 2
#define MASTER_INTENSITY_REG 0x0E
#define MASTER_INTENSITY_VALUE_NONZERO 0xFF
#define CONFIG_REG 0x0F

// Leave blink disabled
// Disable global intensity
// Leave interrupt enabled
#define CONFIG_REGISTER_VALUE 0x08


/** This can be defined to change the behavior of the setOutputPwmValue() function such that passing in 0 will turn the LED off instead of setting it to 100% open drain */
//#define PWM_DC_ACTIVE_LOW

/** 
 * @brief Empty constructor
 */
GpioExpander::GpioExpander() {
  // Nothing to do here
}

/** 
 * @brief Initializes GPIO expander configurations.
 * Note that this assumes all registers are at the initial power up value. Creating a new object for a GPIO Expander already in use will result in undefined behavior.
 * At powerup, all pins are configured as inputs and their output registers are set to high impedance.
 * 
 * @param i2cPipe - I2C interface that has already been initialized
 * @param address - I2C address of the MAX7314 IC
 * @param callback - a function pointer called when the inputs change.  
 *                  Note that this callback occurs in the interrupt thread, and that the pin will remain asserted (preventing any further callbacks) until readInputs() is called.  
 *                  Changing a pin from an output to an input may cause a false interrupt.
 * @param pin - the GPIO pin that the interrupt line is connected to. Unused if callback is NULL
 */
void GpioExpander::init(TwoWire *i2cPipe, GpioExpanderAddress address, void (*callback)(void), uint8_t pin) {
  // Save local variables
  _wire = i2cPipe;
  _address = address;

  // Set up initial configs:
  // Config Register
  // Disable global intensity control so we can do individual PWMs
  // Leave interrupt pin enabled
  _wire->beginTransmission(_address);
  _wire->write(CONFIG_REG);
  _wire->write(CONFIG_REGISTER_VALUE);
  _wire->endTransmission();

  // PWM Master Intensity
  // Set this to a nonzero value to enable PWM
  _wire->beginTransmission(_address);
  _wire->write(MASTER_INTENSITY_REG);
  _wire->write(MASTER_INTENSITY_VALUE_NONZERO);
  _wire->endTransmission();

  // Set up GPIO callback
  if (callback != NULL) {
    //attachInterrupt(pin, callback, FALLING);
    Serial.println("if (callback != NULL)");
    attachInterrupt(digitalPinToInterrupt(pin), callback, FALLING);
  }
}

/** 
 * @brief Configures pins as inputs
 * 
 * @param inputBitfield - a bitfield of pins to be configured as inputs.
 */
void GpioExpander::configureInputs(GpioExpanderStaticPin inputBitfield) {
  // 1 = input, 0 = output
  // Set the bitfield we got in our saved configurations
  _configs[0] |= (inputBitfield & 0xFF);  // Pins 0-7 first
  _configs[1] |= ((inputBitfield >> 8) & 0xFF);

  Serial.print(_configs[0]);
  Serial.print(" ");
  Serial.println(_configs[1]);

  // Store them to the expander
  _wire->beginTransmission(_address);
  _wire->write(PORT_CONFIG_REG_0);
  _wire->write(_configs[0]);
  _wire->write(_configs[1]);
  _wire->endTransmission();
}

/** 
 * @brief Configures pins as outputs
 * 
 * @param outputBitfield - a bitfield of pins to be configured as outputs.
 */
void GpioExpander::configureOutputs(GpioExpanderStaticPin outputBitfield) {
  // 1 = input, 0 = output
  // Clear the bitfield we got from our saved configurations
  _configs[0] &= ~(outputBitfield & 0xFF);  // Pins 0-7 first
  _configs[1] &= ~((outputBitfield >> 8) & 0xFF);

  // Store them to the expander
  _wire->beginTransmission(_address);
  _wire->write(PORT_CONFIG_REG_0);
  _wire->write(_configs[0]);
  _wire->write(_configs[1]);
  _wire->endTransmission();
}

/** 
 * @brief Reads GPIO input values.
 * 
 * @return a bitfield of pin values that can be parsed with the values in the GpioExpanderStaticPin enum.
 */
uint16_t GpioExpander::readInputs() {
  uint16_t inputRead = 0;

  // Set the register to read from
  _wire->beginTransmission(_address);
  _wire->write(INPUT_REG_0);
  _wire->endTransmission();
  _wire->requestFrom((uint8_t)_address, (uint8_t)2);

#ifdef DEBUG
  char result[80];
  snprintf(result, 80, "GpioExpander::readInputs(): <0x%.2X> <0x%.2X> <0x%.2X> <0x%.2X> ", _address, OUTPUT_REG_0, _staticOutputs[0], _staticOutputs[1]);
  Serial.println(String(result));
#endif


  // Make sure we got a response
  if (_wire->available() >= 2) {
    inputRead = _wire->read();  // Pins 0-7 first
    inputRead |= ((_wire->read()) << 8);
  }
  return inputRead;
}

/** 
 * @brief Sets pins configured as static GPIO outputs to high impedance.  
 * If any pins have been set to PWM values, they should be set to either 0% or 100% duty cycle before they are controlled using this function.
 * 
 * @param pinSetBitfield - a bitfield of pins to be set high (high impedance).
 */
void GpioExpander::setStaticOutputs(GpioExpanderStaticPin pinSetBitfield) {
  // Set the bitfield we got in our saved outputs
  _staticOutputs[0] |= (pinSetBitfield & 0xFF);  // Pins 0-7 first
  _staticOutputs[1] |= ((pinSetBitfield >> 8) & 0xFF);

  // Write them to the expander
  _wire->beginTransmission(_address);
  _wire->write(OUTPUT_REG_0);
  _wire->write(_staticOutputs[0]);
  _wire->write(_staticOutputs[1]);
  _wire->endTransmission();

#ifdef DEBUG
  char result[80];
  snprintf(result, 80, "GpioExpander::setStaticOutputs(0x%.4X): <0x%.2X> <0x%.2X> <0x%.2X> <0x%.2X> ", pinSetBitfield, _address, OUTPUT_REG_0, _staticOutputs[0], _staticOutputs[1]);
  Serial.println(String(result));
#endif
}

/** 
 * @brief Drives pins configured as static GPIO outputs low.  
 * If any pins have been set to PWM values, they should be set to either 0% or 100% duty cycle before they are controlled using this function.
 * 
 * @param pinClearBitfield - a bitfield of pins to be set low (open drain).
 */
void GpioExpander::clearStaticOutputs(GpioExpanderStaticPin pinClearBitfield) {
  // Clear the bitfield we got from  our saved outputs
  _staticOutputs[0] &= ~(pinClearBitfield & 0xFF);  // Pins 0-7 first
  _staticOutputs[1] &= ~((pinClearBitfield >> 8) & 0xFF);

  // Write them to the expander
  _wire->beginTransmission(_address);
  _wire->write(OUTPUT_REG_0);
  _wire->write(_staticOutputs[0]);
  _wire->write(_staticOutputs[1]);
  _wire->endTransmission();

#ifdef DEBUG
  char result[80];
  snprintf(result, 80, "GpioExpander::clearStaticOutputs(0x%.4X): <0x%.2X> <0x%.2X> <0x%.2X> <0x%.2X> ", pinClearBitfield, _address, OUTPUT_REG_0, _staticOutputs[0], _staticOutputs[1]);
  Serial.println(String(result));
#endif
}

/** 
 * @brief Sets PWM duty cycle of pins set to GPIO outputs
 * 
 * @param pin - the pin to set the value of. PWM functions must be called on one pin at a time. The function has no effect if the pin is not configured as an output.
 * @param dutyCycle - duty cyle of the PWM in 1/16 increments.  
 *                  0 will drive the pin low for 100% duty cycle.  
 *                  8 will drive the pin low for 50% duty cycle and set it to high impedance for 50% duty cycle.  
 *                  16 will set the pin to high impedance for 100% duty cycle.
 */
void GpioExpander::setOutputPwmValue(GpioExpanderPwmPin pin, uint8_t dutyCycle) {
#ifdef PWM_DC_ACTIVE_LOW
  dutyCycle = (dutyCycle >= 16) ? 0 : 16 - dutyCycle;
#endif

  // Find the PWM index (register offset)
  uint8_t pwmIndex = (pin / PINS_PER_PWM_REGISTER);
  bool upperNybble = (pin % PINS_PER_PWM_REGISTER);  // Even pins are lower nybble, odd pins are upper nybble
  uint8_t leftShift = upperNybble ? 4 : 0;           // Determine how much we should shift by depending which nybble we want to write to
  uint8_t regValue = dutyCycle - 1;                  // Value that goes into the register is offset by 1 vs actual duty cycle (0 indexed)

  // Zero and 100% duty cycle are special cases
  // PWm register is set to 0x0F for both, but how the static output register is configured determines whether it's 0 or 100%
  if (dutyCycle == 0) {
    regValue = 0x0F;
    // Clear the bit in the static output register
    clearStaticOutputs((GpioExpanderStaticPin)(1 << pin));  // Shift to convert the GpioExpanderPwmPin to a GpioExpanderStaticPin
  } else if (dutyCycle >= 16) {
    regValue = 0x0F;
    // Set the bit in the static output register
    setStaticOutputs((GpioExpanderStaticPin)(1 << pin));  // Shift to convert the GpioExpanderPwmPin to a GpioExpanderStaticPin
  } else {
    // Set the bit in the static output register
    // This is only really necessary if it was 0% before, but it's cleaner code to just do it and it doesn't cost us much
    setStaticOutputs((GpioExpanderStaticPin)(1 << pin));  // Shift to convert the GpioExpanderPwmPin to a GpioExpanderStaticPin
  }

  // Set the PWM value
  // Clear the field in the register
  _pwmOutputs[pwmIndex] &= ~(0x0F << leftShift);
  // Set the new value
  _pwmOutputs[pwmIndex] |= (regValue << leftShift);

  // Write to the expander
  _wire->beginTransmission(_address);
  _wire->write(PWM_REG_0 + pwmIndex);
  _wire->write(_pwmOutputs[pwmIndex]);
  _wire->endTransmission();

#ifdef DEBUG
  char result[80];
  snprintf(result, 80, "GpioExpander::setOutputPwmValue(0x%.4X, %d): <0x%.2X> <0x%.2X> <0x%.2X> <0x%.2X> ", pin, dutyCycle, _address, OUTPUT_REG_0, _staticOutputs[0], _staticOutputs[1]);
  Serial.println(String(result));
#endif
}
