#ifndef PINMAP_H
#define PINMAP_H
/*
  Pin Map for MAX7314 GPIO Expander IC
*/

/*
  Enum used to reference pins for static (non PWM) GPIO control.
  These can be used as a bitfield,
  passing multiple of them into a function at the same time.
*/
enum StaticPins
{
  STATIC_PIN0 = 0x0001,
  STATIC_PIN1 = 0x0002,
  STATIC_PIN2 = 0x0004,
  STATIC_PIN3 = 0x0008,
  STATIC_PIN4 = 0x0010,
  STATIC_PIN5 = 0x0020,
  STATIC_PIN6 = 0x0040,
  STATIC_PIN7 = 0x0080,
  STATIC_PIN8 = 0x0100,
  STATIC_PIN9 = 0x0200,
  STATIC_PIN10 = 0x0400,
  STATIC_PIN11 = 0x0800,
  STATIC_PIN12 = 0x1000,
  STATIC_PIN13 = 0x2000,
  STATIC_PIN14 = 0x4000,
  STATIC_PIN15 = 0x8000,
};

/*
  Enum used to reference pins for PWM output.
  Known PWM pins on Expander 1 are given aliases for convenience.
*/
enum PwmPins
{
  PWM_PIN0 = 0,
  PWM_PIN1 = 1,
  PWM_PIN2 = 2,
  PWM_PIN3 = 3,
  PWM_PIN4 = 4,
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

// enum MAX7314_Pins
// {
//   PIN0 = 0x0001,
//   DPUMP_PWR_EN_N = PIN0, // J19
//   MDM_N_RESET = PIN0,    // J34_5

//   PIN1 = 0x0002,
//   MAIN_PWR_EN = PIN1, // J22
//   MDM_ON_OFF = PIN1,  // J35_10

//   PIN2 = 0x0004,
//   LED_R = PIN2,   // J26_5
//   MUX_SEL = PIN2, // U34_SEL

//   PIN3 = 0x0008,
//   LED_G = PIN3,     // J26_6
//   BOOT_DONE = PIN3, // U4_2OE

//   PIN4 = 0x0010,
//   LED_B = PIN4,         // J26_7
//   EXP2_SPARE_P4 = PIN4, // NIU

//   PIN5 = 0x0020,
//   DSENS_PWR_EN = PIN5,  // J18_4
//   EXP2_SPARE_P5 = PIN5, // NIU

//   PIN6 = 0x0040,
//   DENS_EN = PIN6,          // J17_3
//   MDM_VREF_PWREN_N = PIN6, // Q13

//   PIN7 = 0x0080,
//   DENS_DIR = PIN7,      // J17_2
//   EXP2_SPARE_P7 = PIN7, // NIU

//   PIN8 = 0x0100,
//   MDM_STATUS = PIN8,   // U18_4
//   CS_VALVE1_EN = PIN8, // J2

//   PIN9 = 0x0200,
//   DIP_SW1 = PIN9,
//   CS_12VPWM1_EN = PIN9, // J3

//   PIN10 = 0x0400,
//   DIP_SW2 = PIN10,
//   CS_12VPWM2_EN = PIN10, // J4

//   PIN11 = 0x0800,
//   DIP_SW3 = PIN11,
//   CS_12VPWM3_EN = PIN11, // J6

//   PIN12 = 0x1000,
//   DIP_SW4 = PIN12,
//   CS_12VPWM4_EN = PIN12, // J8;

//   PIN13 = 0x2000,
//   LEAK_ALARM = PIN13,   // J25
//   CS_VALVE6_EN = PIN13, // J5

//   PIN14 = 0x4000,
//   CS_LEVEL_N = PIN14,   // J11
//   CS_VALVE7_EN = PIN14, // J7

//   PIN15 = 0x8000,
//   LED_TRIG = PIN15,     // J26_3
//   CS_VALVE8_EN = PIN15, // J9
// };

#endif
