#ifndef PIN_H_
#define PIN_H_

#include <stdint.h>

// arduino pin numbers
#define PIN_B0_ARD  8
#define PIN_B1_ARD  9
#define PIN_B2_ARD 10
#define PIN_B3_ARD 11
#define PIN_B4_ARD 12
#define PIN_B5_ARD 13
#define PIN_C0_ARD 14
#define PIN_C1_ARD 15
#define PIN_C2_ARD 16
#define PIN_C3_ARD 17
#define PIN_C4_ARD 18
#define PIN_C5_ARD 19
#define PIN_D0_ARD  0
#define PIN_D1_ARD  1
#define PIN_D2_ARD  2
#define PIN_D3_ARD  3
#define PIN_D4_ARD  4
#define PIN_D5_ARD  5
#define PIN_D6_ARD  6
#define PIN_D7_ARD  7

// pin type
// notes:
// if _SFR_ASM_COMPAT is 0, dereference reg pointers with _SFR_IO8 macro in avr/sfrdefs.h included from avr/io.h
// if _SFR_ASM_COMPAT is 1, dereference reg pointers the standard C way (*)
typedef struct pin_s
        {
	uint8_t           valid_flag; // valid flag
	uint8_t           ard_pin;    // arduino pin number
        volatile uint8_t *pin_reg;    // pinx register pointer
        volatile uint8_t *ddr_reg;    // ddrx register pointer
        volatile uint8_t *port_reg;   // portx register pointer
        uint8_t           pin_bit;    // pin bit position
        uint8_t           pin_mask;   // pin bit mask
        } pin_t;

// flag values
#define PIN_VALID    0xFF
#define PIN_INVALID  0xFE
#define PIN_NOT_USED 0xFF

// ddr input/output
#define PIN_IN  0
#define PIN_OUT 1

// output level low/high
#define PIN_LOW  0
#define PIN_HIGH 1

// pin states
#define PIN_IN         0
#define PIN_IN_HIGHZ   0
#define PIN_IN_PULLUP  1 
#define PIN_OUT_LOW    2 
#define PIN_OUT_HIGH   3 

// functions
int8_t  pin_init(pin_t *pin, volatile uint8_t *pin_reg, uint8_t pin_bit);
int8_t  pin_init_ard(pin_t *pin, uint8_t pin_num);

int8_t  pin_ddr(pin_t *pin, uint8_t pin_value);
int8_t  pin_ddr_ard(uint8_t pin_num, uint8_t pin_value);

int8_t  pin_port(pin_t *pin, uint8_t pin_value);
int8_t  pin_port_ard(uint8_t pin_num, uint8_t pin_value);

int8_t  pin_in(pin_t *pin);
int8_t  pin_in_ard(uint8_t pin_num);

int8_t  pin_state_set(pin_t *pin, uint8_t state);
int8_t  pin_state_set_ard(uint8_t pin_num, uint8_t state);

#endif // PIN_H_
