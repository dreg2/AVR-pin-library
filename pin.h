#ifndef PIN_H_
#define PIN_H_

#include <stdint.h>
#include <stdio.h>

// pin register addresses and arduino pin numbers
#define PIN_B_PTR  ((volatile uint8_t *)0x03)
#define DDR_B_PTR  ((volatile uint8_t *)0x04)
#define PORT_B_PTR ((volatile uint8_t *)0x05)
#define PIN_B0_ARD  8
#define PIN_B1_ARD  9
#define PIN_B2_ARD 10
#define PIN_B3_ARD 11
#define PIN_B4_ARD 12
#define PIN_B5_ARD 13

#define PIN_C_PTR  ((volatile uint8_t *)0x06)
#define DDR_C_PTR  ((volatile uint8_t *)0x07)
#define PORT_C_PTR ((volatile uint8_t *)0x08)
#define PIN_C0_ARD 14
#define PIN_C1_ARD 15
#define PIN_C2_ARD 16
#define PIN_C3_ARD 17
#define PIN_C4_ARD 18
#define PIN_C5_ARD 19

#define PIN_D_PTR  ((volatile uint8_t *)0x09)
#define DDR_D_PTR  ((volatile uint8_t *)0x0A)
#define PORT_D_PTR ((volatile uint8_t *)0x0B)
#define PIN_D0_ARD  0
#define PIN_D1_ARD  1
#define PIN_D2_ARD  2
#define PIN_D3_ARD  3
#define PIN_D4_ARD  4
#define PIN_D5_ARD  5
#define PIN_D6_ARD  6
#define PIN_D7_ARD  7

// pin type
// note: dereference reg pointers with _SFR_IO8 macro in avr/sfrdefs.h included from avr/io.h
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

// functions
int8_t  pin_init(pin_t *pin, volatile uint8_t *pin_reg, uint8_t pin_bit);
int8_t  pin_init_ard(pin_t *pin, uint8_t pin_num);

void    pin_ddr(pin_t *pin, uint8_t ddr);
void    pin_ddr_ard(uint8_t pin_num, uint8_t ddr);

void    pin_out(pin_t *pin, uint8_t out);
void    pin_out_ard(uint8_t pin_num, uint8_t out);

int8_t  pin_in(pin_t *pin);
int8_t  pin_in_ard(uint8_t pin_num);

void    pin_pu(pin_t *pin, uint8_t pu_flag);
void    pin_pu_ard(uint8_t pin_num, uint8_t pu_flag);

// pull-up restistor disable/enable flags
#define PIN_PULLUP_ENABLE  0
#define PIN_PULLUP_DISABLE 1

#endif // PIN_H_
