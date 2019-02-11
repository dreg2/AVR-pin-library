#ifndef PIN_H_
#define PIN_H_

#include <stdint.h>
#include <stdio.h>

// pin register addresses - works for atmega328p
#define PIN_B  ((volatile uint8_t *)0x03)
#define DDR_B  ((volatile uint8_t *)0x04)
#define PORT_B ((volatile uint8_t *)0x05)
#define PIN_C  ((volatile uint8_t *)0x06)
#define DDR_C  ((volatile uint8_t *)0x07)
#define PORT_C ((volatile uint8_t *)0x08)
#define PIN_D  ((volatile uint8_t *)0x09)
#define DDR_D  ((volatile uint8_t *)0x0A)
#define PORT_D ((volatile uint8_t *)0x0B)

// avr pin type
// note: derference reg pointers with _SFR_IO8 macro in avr/sfrdefs.h included from avr/io.h
typedef struct avr_pin_s
        {
        volatile uint8_t *pin_reg;   // pinx register pointer
        volatile uint8_t *ddr_reg;   // ddrx register pointer
        volatile uint8_t *port_reg;  // portx register pointer
        uint8_t           pin_bit;   // pin bit position
        uint8_t           pin_mask;  // pin bit mask
	} avr_pin_t;

// pin type
typedef struct pin_s
        {
	uint8_t   ard_pin;   // arduino pin number
	avr_pin_t avr_pin;   // avr pin structure
        } pin_t;

// ddr input/output
#define PIN_IN  0
#define PIN_OUT 1

// output level low/high
#define PIN_LOW  0
#define PIN_HIGH 1

// pull-up restistor disable/enable
#define PIN_PUD 0
#define PIN_PUE 1

// functions
int8_t  pin_init(pin_t *pin, volatile uint8_t *pin_reg, uint8_t pin_bit);
int8_t  pin_init_ard(pin_t *pin, uint8_t pin_num);

void    pin_ddr(pin_t *pin, uint8_t ddr);
void    pin_ddr_ard(uint8_t pin_num, uint8_t ddr);

void    pin_out(pin_t *pin, uint8_t out);
void    pin_out_ard(uint8_t pin_num, uint8_t out);

int8_t  pin_in(pin_t *pin);
int8_t  pin_in_ard(uint8_t pin_num);

void    pin_pue(pin_t *pin, uint8_t pue);
void    pin_pue_ard(uint8_t pin_num, uint8_t pue);

#endif // PIN_H_
