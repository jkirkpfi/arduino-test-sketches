/*
  Library for MAX7314 GPIO Expander
*/
#include "MAX7314.hpp"
#include "Arduino.h"

// Uncomment if using Arduino IDE
// MAX7314::MAX7314() {}

void MAX7314::init(
    TwoWire *i2c_interface,
    uint8_t i2c_address,
    void (*interrupt_callback)(void),
    uint8_t interrupt_pin)
{

  _i2c_interface = i2c_interface;
  _i2c_address = i2c_address;

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
  if (interrupt_callback != NULL)
  {
    attachInterrupt(digitalPinToInterrupt(interrupt_pin), interrupt_callback, FALLING);
  }
}

void MAX7314::configurePins(uint16_t bit_field)
{
  uint8_t low_bits = (bit_field & 0xff);
  uint8_t high_bits = (bit_field >> 8);

  _i2c_interface->beginTransmission(_i2c_address);
  _i2c_interface->write(PORT_CONFIG_REGISTER_0);
  _i2c_interface->write(low_bits);
  _i2c_interface->write(high_bits);
  _i2c_interface->endTransmission();
}

uint16_t MAX7314::readPins()
{
  uint16_t _input_vals = 0;

  // Set the register to read from
  _i2c_interface->beginTransmission(_i2c_address);
  _i2c_interface->write(INPUT_REGISTER_0);
  _i2c_interface->endTransmission();
  _i2c_interface->requestFrom(_i2c_address, _kInputRequestSize);

  // Make sure we got a response
  if (_i2c_interface->available() >= _kInputRequestSize)
  {
    // Read pins 0-7 first.
    _input_vals = _i2c_interface->read();
    _input_vals |= ((_i2c_interface->read()) << 8);
  }

  return _input_vals;
}

void MAX7314::setPinsHigh(uint16_t bit_field)
{
  // Set pins 0-7 first.
  _output_states[0] |= (bit_field & 0xff);
  _output_states[1] |= (bit_field >> 8);

  _i2c_interface->beginTransmission(_i2c_address);
  _i2c_interface->write(OUTPUT_REGISTER_0);
  _i2c_interface->write(_output_states[0]);
  _i2c_interface->write(_output_states[1]);
  _i2c_interface->endTransmission();
}

void MAX7314::setPinsLow(uint16_t bit_field)
{
  // Clear the bitfield we got from  our saved outputs
  // Set pins 0-7 first.
  _output_states[0] &= ~(bit_field & 0xFF);
  _output_states[1] &= ~(bit_field >> 8);

  _i2c_interface->beginTransmission(_i2c_address);
  _i2c_interface->write(OUTPUT_REGISTER_0);
  _i2c_interface->write(_output_states[0]);
  _i2c_interface->write(_output_states[1]);
  _i2c_interface->endTransmission();
}
