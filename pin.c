#include <avr/io.h>
#include <stdio.h>

#include "common.h"
#include "pin.h"

#define PORT_B_IDX 0
#define PORT_C_IDX 1
#define PORT_D_IDX 2

// avr pin bank address look-up table
struct avr_port_lut_s
        {
        volatile uint8_t *pin_reg;   // pinx register pointer
        volatile uint8_t *ddr_reg;   // ddrx register pointer
        volatile uint8_t *port_reg;  // portx register pointer
        } avr_port_lut[] =
		{
		{PIN_B, DDR_B, PORT_B},
		{PIN_C, DDR_C, PORT_C},
		{PIN_D, DDR_D, PORT_D}
		};

// pin look up table - arduino pin number is the index
struct pin_lut_t
	{
        uint8_t   avr_port;  // avr pin bank
	uint8_t   avr_bit;   // avr bit position
	} pin_lut[] =
		{
		{PORT_D_IDX, PIND0},
		{PORT_D_IDX, PIND1},
		{PORT_D_IDX, PIND2},
		{PORT_D_IDX, PIND3},
		{PORT_D_IDX, PIND4},
		{PORT_D_IDX, PIND5},
		{PORT_D_IDX, PIND6},
		{PORT_D_IDX, PIND7},
		{PORT_B_IDX, PINB0},
		{PORT_B_IDX, PINB1},
		{PORT_B_IDX, PINB2},
		{PORT_B_IDX, PINB3},
		{PORT_B_IDX, PINB4},
		{PORT_B_IDX, PINB5},
		{PORT_C_IDX, PINC0},
		{PORT_C_IDX, PINC1},
		{PORT_C_IDX, PINC2},
		{PORT_C_IDX, PINC3},
		{PORT_C_IDX, PINC4},
		{PORT_C_IDX, PINC5},
		};

//----------------------------------------------------------------------------------------------------
// initialize pin structure
//----------------------------------------------------------------------------------------------------
int8_t pin_init(pin_t *pin, volatile uint8_t *pin_reg, uint8_t pin_bit)
	{
	pin->valid_flag = PIN_INVALID;
	pin->ard_pin    = PIN_NOT_USED;

	// validate pin_reg
	uint8_t i;
	for (i = 0; i <  ARRAY_SIZE(avr_port_lut); i++)
		if (avr_port_lut[i].pin_reg == pin_reg) // found
			break;
	if (i >= 3) // not found
		return -1;

	// validate pin_bit
	if (pin_bit > 7)
		return -1;

	// lookup and validate arduino pin num
	for (i = 0; i < ARRAY_SIZE(pin_lut); i++)
		if (avr_port_lut[pin_lut[i].avr_port].pin_reg == pin_reg && pin_lut[i].avr_bit == pin_bit) // found
			break;
	if (i >= 20) // not found
		return -1;

	// assign pin struct values
	pin->ard_pin  = i;
	pin->pin_reg  = pin_reg;
	pin->ddr_reg  = (volatile uint8_t *)(pin_reg + 0x01);
	pin->port_reg = (volatile uint8_t *)(pin_reg + 0x02);
	pin->pin_bit  = pin_bit;
	pin->pin_mask = (uint8_t)(_BV(pin_bit));

	pin->valid_flag = PIN_VALID;
	return 0;
	}

//----------------------------------------------------------------------------------------------------
// initialize pin structure from arduino pin number
//----------------------------------------------------------------------------------------------------
int8_t pin_init_ard(pin_t *pin, uint8_t pin_num)
	{
	pin->valid_flag = PIN_INVALID;
	pin->ard_pin    = PIN_NOT_USED;

	// validate pin_num
	if (pin_num > 19)
		return -1;

	// assign pin struct values from look-up table
	pin->ard_pin  = pin_num;
	pin->pin_reg  = avr_port_lut[pin_lut[pin_num].avr_port].pin_reg;
	pin->ddr_reg  = avr_port_lut[pin_lut[pin_num].avr_port].ddr_reg;
	pin->port_reg = avr_port_lut[pin_lut[pin_num].avr_port].port_reg;
	pin->pin_bit  = pin_lut[pin_num].avr_bit;
	pin->pin_mask = (uint8_t)(_BV(pin->pin_bit));

	pin->valid_flag = PIN_VALID;
	return 0;
	}

//----------------------------------------------------------------------------------------------------
// pin data direction
//----------------------------------------------------------------------------------------------------
void pin_ddr(pin_t *pin, uint8_t ddr)
	{
	// check validity
	if (pin->valid_flag != PIN_VALID)
		return;

	// set/clear bit in ddr register
	if (ddr == PIN_OUT)
		_SFR_IO8(pin->ddr_reg) |= pin->pin_mask;
	else
		_SFR_IO8(pin->ddr_reg) &= (uint8_t)~pin->pin_mask;
	}

//----------------------------------------------------------------------------------------------------
// pin data direction from arduino pin number
//----------------------------------------------------------------------------------------------------
void pin_ddr_ard(uint8_t pin_num, uint8_t ddr)
	{
	// initialize pin struct
	pin_t pin;
	if (pin_init_ard(&pin, pin_num) < 0)
		return;

	// set/clear bit in ddr register
	if (ddr == PIN_OUT)
		_SFR_IO8(pin.ddr_reg) |= pin.pin_mask;
	else
		_SFR_IO8(pin.ddr_reg) &= (uint8_t)~pin.pin_mask;
	}

//----------------------------------------------------------------------------------------------------
// set output pin level
//----------------------------------------------------------------------------------------------------
void pin_out(pin_t *pin, uint8_t out)
	{
	// check validity
	if (pin->valid_flag != PIN_VALID)
		return;

	// set/clear bit in port register
	if (out == PIN_HIGH)
		_SFR_IO8(pin->port_reg) |= pin->pin_mask;
	else
		_SFR_IO8(pin->port_reg) &= (uint8_t)~pin->pin_mask;
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
		_SFR_IO8(pin.port_reg) |= pin.pin_mask;
	else
		_SFR_IO8(pin.port_reg) &= (uint8_t)~pin.pin_mask;
	}

//----------------------------------------------------------------------------------------------------
// return pin level
//----------------------------------------------------------------------------------------------------
int8_t pin_in(pin_t *pin)
	{
	// check validity
	if (pin->valid_flag != PIN_VALID)
		return -1;

	// return bit from pin register
	return (int8_t)(_SFR_IO8(pin->pin_reg) & pin->pin_mask);
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
	return (int8_t)(_SFR_IO8(pin.pin_reg) & pin.pin_mask);
	}

//----------------------------------------------------------------------------------------------------
// enable/disable pin pull-up restistors
//----------------------------------------------------------------------------------------------------
void pin_pu(pin_t *pin, uint8_t pu_flag)
	{
	// check validity
	if (pin->valid_flag != PIN_VALID)
		return;

	// save ddr register
	uint8_t ddr_save = _SFR_IO8(pin->ddr_reg);

	// set pin for input
	_SFR_IO8(pin->ddr_reg) &= (uint8_t)~pin->pin_mask;

	// enable/disable pull-up by setting/clearing output bit
	if (pu_flag == PIN_PULLUP_ENABLE)
		_SFR_IO8(pin->port_reg) |= pin->pin_mask;
	else
		_SFR_IO8(pin->port_reg) &= (uint8_t)~pin->pin_mask;

	// restore ddr register
	_SFR_IO8(pin->ddr_reg) = ddr_save;
	}

//----------------------------------------------------------------------------------------------------
// enable/disable pin pull-up restistors from arduino pin number
//----------------------------------------------------------------------------------------------------
void pin_pu_ard(uint8_t pin_num, uint8_t pu_flag)
	{
	// initialize pin struct
	pin_t pin;
	if (pin_init_ard(&pin, pin_num) < 0) return;

	// save ddr register
	uint8_t ddr_save = _SFR_IO8(pin.ddr_reg);

	// set pin for input
	_SFR_IO8(pin.ddr_reg) &= (uint8_t)~pin.pin_mask;

	// enable/disable pull-up by setting/clearing output bit
	if (pu_flag == PIN_PULLUP_ENABLE)
		_SFR_IO8(pin.port_reg) |= pin.pin_mask;
	else
		_SFR_IO8(pin.port_reg) &= (uint8_t)~pin.pin_mask;

	// restore ddr register
	_SFR_IO8(pin.ddr_reg) = ddr_save;
	}
