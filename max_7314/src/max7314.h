/*
  Library for MAX7314 GPIO Expander
*/

#ifndef MAX7314_H
#define MAX7314_H

#include "Arduino.h"
#include <Wire.h>

// #define GPIO_EXPANDER_VERSION "MAX7314 Driver v1.0.1\r\n"
// #define DEBUG

/*
  Enum of GPIO expander I2C addresses.
*/
enum MAX7314_Address {
  EXPANDER_1 = 0x20,  // Inputs, and PWM and GPIO outputs
  EXPANDER_2 = 0x24,  // GPIO outputs only
};

/*
  Enum used to reference pins for static (non PWM) GPIO control.
  These can be used as a bitfield,
  passing multiple of them into a function at the same time.
  Note that pins are given aliases that match their location on the schematic,
  but are not separated between the two expanders.
*/
enum MAX7314_StaticPins {
  STATIC_PIN0 = 0x0001,
  // STATIC_PIN_DPUMP_PWR_EN = STATIC_PIN0,
  // STATIC_PIN_MDM_N_RESET = STATIC_PIN0,
  STATIC_PIN1 = 0x0002,
  // STATIC_PIN_MAIN_PWR_EN = STATIC_PIN1,
  // STATIC_PIN_MDM_ON_OFF = STATIC_PIN1,
  STATIC_PIN2 = 0x0004,
  // STATIC_PIN_LED_R = STATIC_PIN2,
  // STATIC_PIN_MUX_SEL = STATIC_PIN2,
  STATIC_PIN3 = 0x0008,
  // STATIC_PIN_LED_G = STATIC_PIN3,
  // STATIC_PIN_BOOT_DONE = STATIC_PIN3,
  STATIC_PIN4 = 0x0010,
  // STATIC_PIN_LED_B = STATIC_PIN4,
  // STATIC_PIN_RS485_DE = STATIC_PIN4,
  STATIC_PIN5 = 0x0020,
  // STATIC_PIN_DSENS_PWR_EN = STATIC_PIN5,
  // STATIC_PIN_EXP2_SPARE_P5 = STATIC_PIN5,
  STATIC_PIN6 = 0x0040,
  // STATIC_PIN_DENS_EN = STATIC_PIN6,
  // STATIC_PIN_MDM_VREF_PWREN_N = STATIC_PIN6,
  STATIC_PIN7 = 0x0080,
  // STATIC_PIN_DENS_DIR = STATIC_PIN7,
  // STATIC_PIN_EXP2_SPARE_P7 = STATIC_PIN7,

  STATIC_PIN8 = 0x0100,
  // STATIC_PIN_MDM_STATUS = STATIC_PIN8,
  // STATIC_PIN_CS_VALVE1_EN = STATIC_PIN8,
  STATIC_PIN9 = 0x0200,
  // STATIC_PIN_DIP_SW1 = STATIC_PIN9,
  // STATIC_PIN_CS_VALVE2_EN = STATIC_PIN9,
  STATIC_PIN10 = 0x0400,
  // STATIC_PIN_DIP_SW2 = STATIC_PIN10,
  // STATIC_PIN_CS_VALVE3_EN = STATIC_PIN10,
  STATIC_PIN11 = 0x0800,
  // STATIC_PIN_DIP_SW3 = STATIC_PIN11,
  // STATIC_PIN_CS_VALVE4_EN = STATIC_PIN11,
  STATIC_PIN12 = 0x1000,
  // STATIC_PIN_DIP_SW4 = STATIC_PIN12,
  // STATIC_PIN_CS_VALVE5_EN = STATIC_PIN12,
  STATIC_PIN13 = 0x2000,
  // STATIC_PIN_EXP1_SPARE_P13 = STATIC_PIN13,
  // STATIC_PIN_CS_VALVE6_EN = STATIC_PIN13,
  STATIC_PIN14 = 0x4000,
  // STATIC_PIN_CS_LEVEL_N = STATIC_PIN14,
  // STATIC_PIN_CS_VALVE7_EN = STATIC_PIN14,
  STATIC_PIN15 = 0x8000,
  // STATIC_PIN_LED_TRIG = STATIC_PIN15,
  // STATIC_PIN_CS_VALVE8_EN = STATIC_PIN15,
};

/*
  Enum used to reference pins for PWM output.
  Known PWM pins on Expander 1 are given aliases for convenience.
*/
enum MAX7314_PwmPins {
  PWM_PIN0 = 0,
  // PWM_PIN_DPUMP_PWR_EN = PWM_PIN0,
  PWM_PIN1 = 1,
  // PWM_PIN_MAIN_PWR_EN = PWM_PIN1,
  PWM_PIN2 = 2,
  // PWM_PIN_LED_R = PWM_PIN2,
  PWM_PIN3 = 3,
  // PWM_PIN_LED_G = PWM_PIN3,
  PWM_PIN4 = 4,
  // PWM_PIN_LED_B = PWM_PIN4,
  PWM_PIN5 = 5,
  PWM_PIN6 = 6,
  PWM_PIN7 = 7,
  PWM_PIN8 = 8,
  PWM_PIN9 = 9,
  PWM_PIN10 = 10,
  PWM_PIN11 = 11,
  PWM_PIN12 = 12,
  PWM_PIN13 = 13,
  PWM_PIN14 = 14,
  PWM_PIN15 = 15,
};

class MAX7314 {
  public:
  /** @brief Empty constructor. 
   * 
   * Arduino standard seems to be having a constructor that does very little 
   * and an init() or begin() function that handles setup.
   */
  MAX7314();

  /** 
   * @brief Initializes GPIO expander configurations.
   * 
   * Note that this assumes all registers are at the initial power up value. 
   * Creating a new object for a GPIO Expander already in use 
   * will result in undefined behavior.
   * At powerup, all pins are configured as inputs 
   * and their output registers are set to high impedance.
   * 
   * @param i2cInterface   I2C interface
   * @param i2cAddress   MAX7314 I2C address
   * @param callback  A function pointer called when the inputs change.  
   *                  Note that this callback occurs in the interrupt thread, 
   *                  and that the pin will remain asserted 
   *                  preventing any further callbacks 
   *                  until readInputs() is called.  
   *                  Changing a pin from an output to an input 
   *                  may cause a false interrupt.
   * @param pin The GPIO pin that the interrupt line is connected to. 
   *            Unused if callback is NULL
   */
  void init(
    TwoWire* i2c_interface,
    MAX7314_Address i2c_address,
    void (*callback)(void),
    uint8_t pin
  );

  /** 
   * @brief Configures pins as inputs
   * 
   * @param inputBitfield A bitfield of pins to be configured as inputs.
   */
  void configureInputs(MAX7314_StaticPins inputBitfield);

  /** 
   * @brief Configures pins as outputs
   * 
   * @param outputBitfield A bitfield of pins to be configured as outputs.
   */
  void configureOutputs(MAX7314_StaticPins outputBitfield);

  /** 
   * @brief Reads GPIO input values.
   * 
   * @return A bitfield of pin values.
   */
  uint16_t readInputs();

  /** 
   * @brief Sets pins configured as static GPIO outputs to high.
   * 
   * If any pins have been set to PWM values,
   * they should be set to either 0% or 100% duty cycle
   * before they are controlled using this function.
   * 
   * @param pinSetBitfield A bitfield of pins to be set high.
   */
  void setStaticOutputs(MAX7314_StaticPins pinSetBitfield);

  /** 
   * @brief Sets pins configured as static GPIO outputs low.  
   * 
   * If any pins have been set to PWM values, 
   * they should be set to either 0% or 100% duty cycle 
   * before they are controlled using this function.
   * 
   * @param pinClearBitfield - A bitfield of pins to be set low.
   */
  void clearStaticOutputs(MAX7314_StaticPins pinClearBitfield);

  /** 
   * @brief Sets PWM duty cycle of pins set to GPIO outputs
   * 
   * PWM functions must be called on one pin at a time. 
   * The function has no effect if the pin is not configured as an output.
   * 
   * @param pin The pin to set the value of. 
   * @param dutyCycle Duty cyle of the PWM in 1/16 increments.  
   *                  0 will drive the pin low for 100% duty cycle.  
   *                  8 will drive the pin low for 50% duty cycle 
   *                  and set it to high for 50% duty cycle.  
   *                  16 will set the pin to high impedance for 100% duty cycle.
   */
  void setOutputPwmValue(MAX7314_PwmPins pin, uint8_t dutyCycle);

private:
  // I2C instance to use for communication
  TwoWire* _i2c_interface;
  // I2C address for this GpioExpander object
  uint8_t _i2c_address;
  // Pin input/output configurations. Initialized to default powerup value. 
  uint8_t _configs[2] = { 0xFF, 0xFF };
  // Pin static output values. Initialized to default powerup value.
  uint8_t _staticOutputs[2] = { 0xFF, 0xFF };
  // Pin PWM output values. Initialized to default powerup value. 
  uint8_t _pwmOutputs[8] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
};

#endif
