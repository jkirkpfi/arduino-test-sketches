/*
  Library for MAX7314 GPIO Expander
*/

#include "Arduino.h"
#include "max7314.h"

// The GPIO Expander auto-increments registers, so for any _REG_0 register,
// we don't need to define any others
// #define INPUT_REG_0 0x00
// #define OUTPUT_REG_0 0x02
// #define PORT_CONFIG_REG_0 0x06
// #define PWM_REG_0 0x10
// #define PINS_PER_PWM_REGISTER 2
// #define MASTER_INTENSITY_REG 0x0E
// #define MASTER_INTENSITY_VALUE_NONZERO 0xFF
// #define CONFIG_REG 0x0F

// Leave blink disabled
// Disable global intensity
// Leave interrupt enabled
// #define CONFIG_REGISTER_VALUE 0x08

/** This can be defined to change the behavior of the setOutputPwmValue() 
 * function such that passing in 0 will turn the LED off 
 * instead of setting it to 100% open drain */
//#define PWM_DC_ACTIVE_LOW

enum Registers {
  INPUT_REGISTER_0 = 0x00,
  OUTPUT_REGISTER_0 = 0x02,
  PORT_CONFIG_REGISTER_0 = 0x06,
  PWM_REGISTER_0 = 0x10,
  PINS_PER_PWM_REGISTER = 2,
  MASTER_INTENSITY_REGISTER = 0x0E,
  MASTER_INTENSITY_VALUE_NONZERO = 0xFF,
  CONFIG_REGISTER = 0x0F,
  CONFIG_REGISTER_VALUE = 0x08,
};

/** 
 * @brief Empty constructor
 */
MAX7314::MAX7314() {
  // Leave empty.
}

void MAX7314::init(
  TwoWire *i2cInterface,
  MAX7314_Address i2cAddress,
  void (*callback)(void),
  uint8_t pin) {
  // Save local variables
  _i2c_interface = i2cInterface;
  _i2c_address = i2cAddress;

  // Config Register
  _i2c_interface->beginTransmission(_i2c_address);
  _i2c_interface->write(CONFIG_REGISTER);
  _i2c_interface->write(CONFIG_REGISTER_VALUE);
  _i2c_interface->endTransmission();

  // PWM Master Intensity
  // Set this to a nonzero value to enable PWM.
  _i2c_interface->beginTransmission(_i2c_address);
  _i2c_interface->write(MASTER_INTENSITY_REGISTER);
  _i2c_interface->write(MASTER_INTENSITY_VALUE_NONZERO);
  _i2c_interface->endTransmission();

  // Set up GPIO callback
  if (callback != NULL) {
    // Serial.print("Initializing interupt on GPIO: ");
    // Serial.println(pin);
    attachInterrupt(digitalPinToInterrupt(pin), callback, FALLING);
  }
}

/** 
 * @brief Configures pins as inputs.
 * 
 * @param inputBitfield A bitfield of pins to be configured as inputs.
 */
void MAX7314::configureInputs(MAX7314_StaticPins inputBitfield) {
  // 1 = input, 0 = output
  // Set the bitfield we got in our saved configurations
  _configs[0] |= (inputBitfield & 0xFF);  // Pins 0-7 first
  _configs[1] |= ((inputBitfield >> 8) & 0xFF);

  // int val1 = _configs[0];
  // int val2 = _configs[1];

  Serial.print("PINS 0-7: ");
  Serial.println(_configs[0], BIN);
  Serial.print("PINS 8-15: ");
  Serial.println(_configs[1], BIN);

  // Send them to the expander.
  _i2c_interface->beginTransmission(_i2c_address);
  _i2c_interface->write(PORT_CONFIG_REGISTER_0);
  _i2c_interface->write(_configs[0]);
  _i2c_interface->write(_configs[1]);
  _i2c_interface->endTransmission();
}

/** 
 * @brief Configures pins as outputs
 * 
 * @param outputBitfield A bitfield of pins to be configured as outputs.
 */
void MAX7314::configureOutputs(MAX7314_StaticPins outputBitfield) {
  // 1 = input, 0 = output
  // Clear the bitfield we got from our saved configurations
  _configs[0] &= ~(outputBitfield & 0xFF);  // Pins 0-7 first
  _configs[1] &= ~((outputBitfield >> 8) & 0xFF);

  // int val1 = _configs[0];
  // int val2 = _configs[1];

  Serial.print("PINS 0-7: ");
  Serial.println(_configs[0], BIN);
  Serial.print("PINS 8-15: ");
  Serial.println(_configs[1], BIN);

  // Send them to the expander
  _i2c_interface->beginTransmission(_i2c_address);
  _i2c_interface->write(PORT_CONFIG_REGISTER_0);
  _i2c_interface->write(_configs[0]);
  _i2c_interface->write(_configs[1]);
  _i2c_interface->endTransmission();
}

/** 
 * @brief Reads GPIO input values.
 * 
 * @return a bitfield of pin values that can be parsed 
 *  with the values in the GpioExpanderStaticPin enum.
 */
uint16_t MAX7314::readInputs() {
  uint16_t inputRead = 0;

  // Set the register to read from
  _i2c_interface->beginTransmission(_i2c_address);
  _i2c_interface->write(INPUT_REGISTER_0);
  _i2c_interface->endTransmission();
  _i2c_interface->requestFrom(_i2c_address, (uint8_t)2);

  // #ifdef DEBUG
  //   char result[80];
  //   snprintf(
  //     result,
  //     80,
  //     "GpioExpander::readInputs(): <0x%.2X> <0x%.2X> <0x%.2X> <0x%.2X> ",
  //     _i2c_address,
  //     OUTPUT_REGISTER_0,
  //     _staticOutputs[0],
  //     _staticOutputs[1]
  //   );
  //   Serial.println(String(result));
  // #endif

  // Make sure we got a response
  if (_i2c_interface->available() >= 2) {
    // Read pins 0-7 first.
    inputRead = _i2c_interface->read();
    inputRead |= ((_i2c_interface->read()) << 8);
  }

  return inputRead;
}

void MAX7314::setStaticOutputs(MAX7314_StaticPins pinSetBitfield) {
  // Set the bitfield we got in our saved outputs.
  // Set pins 0-7 first.
  _staticOutputs[0] |= (pinSetBitfield & 0xFF);
  _staticOutputs[1] |= ((pinSetBitfield >> 8) & 0xFF);

  // Write them to the expander
  _i2c_interface->beginTransmission(_i2c_address);
  _i2c_interface->write(OUTPUT_REGISTER_0);
  _i2c_interface->write(_staticOutputs[0]);
  _i2c_interface->write(_staticOutputs[1]);
  _i2c_interface->endTransmission();

  // #ifdef DEBUG
  //   char result[80];
  //   snprintf(
  //     result,
  //     80,
  //     "GpioExpander::setStaticOutputs(0x%.4X): <0x%.2X> <0x%.2X> <0x%.2X> <0x%.2X> ",
  //     pinSetBitfield,
  //     _i2c_address,
  //     OUTPUT_REGISTER_0,
  //     _staticOutputs[0],
  //     _staticOutputs[1]
  //   );
  //   Serial.println(String(result));
  // #endif
}

void MAX7314::clearStaticOutputs(MAX7314_StaticPins pinClearBitfield) {
  // Clear the bitfield we got from  our saved outputs
  // Set pins 0-7 first.
  _staticOutputs[0] &= ~(pinClearBitfield & 0xFF);
  _staticOutputs[1] &= ~((pinClearBitfield >> 8) & 0xFF);

  // Write them to the expander
  _i2c_interface->beginTransmission(_i2c_address);
  _i2c_interface->write(OUTPUT_REGISTER_0);
  _i2c_interface->write(_staticOutputs[0]);
  _i2c_interface->write(_staticOutputs[1]);
  _i2c_interface->endTransmission();

  // #ifdef DEBUG
  //   char result[80];
  //   snprintf(
  //     result,
  //     80,
  //     "GpioExpander::clearStaticOutputs(0x%.4X): <0x%.2X> <0x%.2X> <0x%.2X> <0x%.2X> ",
  //     pinClearBitfield,
  //     _i2c_address,
  //     OUTPUT_REGISTER_0,
  //     _staticOutputs[0],
  //     _staticOutputs[1]
  //   );
  //   Serial.println(String(result));
  // #endif
}

void MAX7314::setOutputPwmValue(MAX7314_PwmPins pin, uint8_t dutyCycle) {
  // #ifdef PWM_DC_ACTIVE_LOW
  //   dutyCycle = (dutyCycle >= 16) ? 0 : 16 - dutyCycle;
  // #endif

  // Find the PWM index.
  uint8_t pwm_index = (pin / PINS_PER_PWM_REGISTER);
  // Even pins are lower nybble, odd pins are upper nybble.
  bool upper_nybble = (pin % PINS_PER_PWM_REGISTER);
  // Determine how much we should shift by depending which nybble to write to.
  uint8_t left_shift = upper_nybble ? 4 : 0;
  // Value that goes into the register is offset by 1 vs actual duty cycle.
  uint8_t reg_value = dutyCycle - 1;

  // Zero and 100% duty cycle are special cases.
  // PWM register is set to 0x0F for both,
  // but how the static output register
  // is configured determines whether it's 0 or 100%.
  if (dutyCycle == 0) {
    reg_value = 0x0F;
    // Clear the bit in the static output register.
    // Shift to convert the MAX7314_PwmPins to a MAX7314_StaticPins.
    clearStaticOutputs((MAX7314_StaticPins)(1 << pin));
  } else if (dutyCycle >= 16) {
    reg_value = 0x0F;
    // Set the bit in the static output register.
    // Shift to convert the MAX7314_PwmPins to a MAX7314_StaticPins.
    setStaticOutputs((MAX7314_StaticPins)(1 << pin));
  } else {
    // Set the bit in the static output register
    // This is only really necessary if it was 0% before,
    // but it's cleaner code to just do it and it doesn't cost us much.
    // Shift to convert the MAX7314_PwmPins to a MAX7314_StaticPins.
    setStaticOutputs((MAX7314_StaticPins)(1 << pin));
  }

  // Set the PWM value
  // Clear the field in the register
  _pwmOutputs[pwm_index] &= ~(0x0F << left_shift);
  // Set the new value
  _pwmOutputs[pwm_index] |= (reg_value << left_shift);

  // Write to the expander
  _i2c_interface->beginTransmission(_i2c_address);
  _i2c_interface->write(PWM_REGISTER_0 + pwm_index);
  _i2c_interface->write(_pwmOutputs[pwm_index]);
  _i2c_interface->endTransmission();

  // #ifdef DEBUG
  //   char result[80];
  //   snprintf(
  //    result,
  //    80,
  //    "GpioExpander::setOutputPwmValue(0x%.4X, %d): <0x%.2X> <0x%.2X> <0x%.2X> <0x%.2X> ",
  //    pin,
  //    dutyCycle,
  //    _address,
  //    OUTPUT_REGISTER_0,
  //    _staticOutputs[0],
  //    _staticOutputs[1]);
  //   Serial.println(String(result));
  // #endif
}
