#include <avr/io.h>
#include <stdio.h>

#include "pin.h"

#define BANK_B 0
#define BANK_C 1
#define BANK_D 2

// avr pin bank address look-up table
struct avr_bank_lut_s
        {
        volatile uint8_t *pin_reg;   // pinx register pointer
        volatile uint8_t *ddr_reg;   // ddrx register pointer
        volatile uint8_t *port_reg;  // portx register pointer
        } avr_bank_lut[] =
		{
		{PIN_B, DDR_B, PORT_B},
		{PIN_C, DDR_C, PORT_C},
		{PIN_D, DDR_D, PORT_D}
		};

// pin look up table - arduino pin number is the index
struct pin_lut_t
	{
        uint8_t   avr_bank;  // avr pin bank
	uint8_t   avr_bit;   // avr bit position
	} pin_lut[] =
		{
		{BANK_D, PIND0},
		{BANK_D, PIND1},
		{BANK_D, PIND2},
		{BANK_D, PIND3},
		{BANK_D, PIND4},
		{BANK_D, PIND5},
		{BANK_D, PIND6},
		{BANK_D, PIND7},
		{BANK_B, PINB0},
		{BANK_B, PINB1},
		{BANK_B, PINB2},
		{BANK_B, PINB3},
		{BANK_B, PINB4},
		{BANK_B, PINB5},
		{BANK_C, PINC0},
		{BANK_C, PINC1},
		{BANK_C, PINC2},
		{BANK_C, PINC3},
		{BANK_C, PINC4},
		{BANK_C, PINC5},
		};

//----------------------------------------------------------------------------------------------------
// initialize pin structure
//----------------------------------------------------------------------------------------------------
int8_t pin_init(pin_t *pin, volatile uint8_t *pin_reg, uint8_t pin_bit)
	{
	// validate pin_reg
	if (pin_reg != PIN_B && pin_reg != PIN_C && pin_reg != PIN_D)
		return -1;

	// validate pin_bit
	if (pin_bit > 7)
		return -1;

	// assign pin struct values
	pin->avr_pin.pin_reg  = pin_reg;
	pin->avr_pin.ddr_reg  = (volatile uint8_t *)(pin_reg + 0x01);
	pin->avr_pin.port_reg = (volatile uint8_t *)(pin_reg + 0x02);
	pin->avr_pin.pin_bit  = pin_bit;
	pin->avr_pin.pin_mask = (uint8_t)(_BV(pin_bit));

	return 0;
	}

//----------------------------------------------------------------------------------------------------
// initialize pin structure from arduino pin number
//----------------------------------------------------------------------------------------------------
int8_t pin_init_ard(pin_t *pin, uint8_t pin_num)
	{
	// validate pin_num
	if (pin_num > 19)
		return -1;

	// assign pin struct values from look-up table
	pin->ard_pin          = pin_num;
	pin->avr_pin.pin_reg  = avr_bank_lut[pin_lut[pin_num].avr_bank].pin_reg;
	pin->avr_pin.ddr_reg  = avr_bank_lut[pin_lut[pin_num].avr_bank].ddr_reg;
	pin->avr_pin.port_reg = avr_bank_lut[pin_lut[pin_num].avr_bank].port_reg;
	pin->avr_pin.pin_bit  = pin_lut[pin_num].avr_bit;
	pin->avr_pin.pin_mask = (uint8_t)(_BV(pin->avr_pin.pin_bit));

	return 0;
	}

//----------------------------------------------------------------------------------------------------
// pin data direction
//----------------------------------------------------------------------------------------------------
void pin_ddr(pin_t *pin, uint8_t ddr)
	{
	// set/clear bit in ddr register
	if (ddr == PIN_OUT)
		_SFR_IO8(pin->avr_pin.ddr_reg) |= pin->avr_pin.pin_mask;
	else
		_SFR_IO8(pin->avr_pin.ddr_reg) &= (uint8_t)~pin->avr_pin.pin_mask;
	}

//----------------------------------------------------------------------------------------------------
// pin data direction from arduino pin number
//----------------------------------------------------------------------------------------------------
void pin_ddr_ard(uint8_t pin_num, uint8_t ddr)
	{
	// initialize pin struct
	pin_t pin;
	if (pin_init_ard(&pin, pin_num) < 0) return;

	// set/clear bit in ddr register
	if (ddr == PIN_OUT)
		_SFR_IO8(pin.avr_pin.ddr_reg) |= pin.avr_pin.pin_mask;
	else
		_SFR_IO8(pin.avr_pin.ddr_reg) &= (uint8_t)~pin.avr_pin.pin_mask;
	}

//----------------------------------------------------------------------------------------------------
// set output pin level
//----------------------------------------------------------------------------------------------------
void pin_out(pin_t *pin, uint8_t out)
	{
	// set/clear bit in port register
	if (out == PIN_HIGH)
		_SFR_IO8(pin->avr_pin.port_reg) |= pin->avr_pin.pin_mask;
	else
		_SFR_IO8(pin->avr_pin.port_reg) &= (uint8_t)~pin->avr_pin.pin_mask;
	}

//----------------------------------------------------------------------------------------------------
// set output pin level from arduino pin number
//----------------------------------------------------------------------------------------------------
void pin_out_ard(uint8_t pin_num, uint8_t out)
	{
	// initialize pin struct
	pin_t pin;
	if (pin_init_ard(&pin, pin_num) < 0) return;

	// set/clear bit in port register
	if (out == PIN_HIGH)
		_SFR_IO8(pin.avr_pin.port_reg) |= pin.avr_pin.pin_mask;
	else
		_SFR_IO8(pin.avr_pin.port_reg) &= (uint8_t)~pin.avr_pin.pin_mask;
	}

//----------------------------------------------------------------------------------------------------
// return pin level
//----------------------------------------------------------------------------------------------------
int8_t pin_in(pin_t *pin)
	{
	// return bit from pin register
	return (int8_t)(_SFR_IO8(pin->avr_pin.pin_reg) & pin->avr_pin.pin_mask);
	}

//----------------------------------------------------------------------------------------------------
// return pin level from arduino pin number
//----------------------------------------------------------------------------------------------------
int8_t pin_in_ard(uint8_t pin_num)
	{
	// initialize pin struct
	pin_t pin;
	if (pin_init_ard(&pin, pin_num) < 0) return -1;

	// return bit from pin register
	return (int8_t)(_SFR_IO8(pin.avr_pin.pin_reg) & pin.avr_pin.pin_mask);
	}

//----------------------------------------------------------------------------------------------------
// enable/disable pin pull-up restistors
//----------------------------------------------------------------------------------------------------
void pin_pue(pin_t *pin, uint8_t pue)
	{
	// save ddr register
	uint8_t ddr_save = _SFR_IO8(pin->avr_pin.ddr_reg);

	// set pin for input
	_SFR_IO8(pin->avr_pin.ddr_reg) &= (uint8_t)~pin->avr_pin.pin_mask;

	// enable/disable pull-up by setting/clearing output bit
	if (pue == PIN_PUE)
		_SFR_IO8(pin->avr_pin.port_reg) |= pin->avr_pin.pin_mask;
	else
		_SFR_IO8(pin->avr_pin.port_reg) &= (uint8_t)~pin->avr_pin.pin_mask;

	// restore ddr register
	_SFR_IO8(pin->avr_pin.ddr_reg) = ddr_save;
	}

//----------------------------------------------------------------------------------------------------
// enable/disable pin pull-up restistors from arduino pin number
//----------------------------------------------------------------------------------------------------
void pin_pue_ard(uint8_t pin_num, uint8_t pue)
	{
	// initialize pin struct
	pin_t pin;
	if (pin_init_ard(&pin, pin_num) < 0) return;

	// save ddr register
	uint8_t ddr_save = _SFR_IO8(pin.avr_pin.ddr_reg);

	// set pin for input
	_SFR_IO8(pin.avr_pin.ddr_reg) &= (uint8_t)~pin.avr_pin.pin_mask;

	// enable/disable pull-up by setting/clearing output bit
	if (pue == PIN_PUE)
		_SFR_IO8(pin.avr_pin.port_reg) |= pin.avr_pin.pin_mask;
	else
		_SFR_IO8(pin.avr_pin.port_reg) &= (uint8_t)~pin.avr_pin.pin_mask;

	// restore ddr register
	_SFR_IO8(pin.avr_pin.ddr_reg) = ddr_save;
	}
