#include <avr/io.h>
#include <stdio.h>
#include <avr/pgmspace.h>

#include "common.h"
#include "pin.h"

#define PORT_B_IDX 0
#define PORT_C_IDX 1
#define PORT_D_IDX 2

// avr pin port register pointers look-up table
const struct avr_port_lut_s
        {
        volatile uint8_t *pin_reg;   // pinx register pointer
        volatile uint8_t *ddr_reg;   // ddrx register pointer
        volatile uint8_t *port_reg;  // portx register pointer
        } PROGMEM avr_port_lut_i[] =
		{
		{PIN_B_PTR, DDR_B_PTR, PORT_B_PTR},
		{PIN_C_PTR, DDR_C_PTR, PORT_C_PTR},
		{PIN_D_PTR, DDR_D_PTR, PORT_D_PTR}
		};

// pin look up table - arduino pin number is the index
const struct pin_lut_s
	{
        uint8_t   avr_port;  // avr pin port
	uint8_t   avr_bit;   // avr bit position
	} PROGMEM pin_lut_i[] =
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
	// set pin to invalid until validated
	pin->valid_flag = PIN_INVALID;
	pin->ard_pin    = PIN_NOT_USED;

	// copy lut arrays from flash memory
	struct avr_port_lut_s avr_port_lut[ARRAY_SIZE(avr_port_lut_i)]; 
        memcpy_P(&avr_port_lut[0], &avr_port_lut_i[0], (ARRAY_SIZE(avr_port_lut)*sizeof(avr_port_lut[0])));
	struct pin_lut_s pin_lut[ARRAY_SIZE(pin_lut_i)]; 
        memcpy_P(&pin_lut[0], &pin_lut_i[0], (ARRAY_SIZE(pin_lut)*sizeof(pin_lut[0])));

	// validate pin_reg
	uint8_t i;
	for (i = 0; i <  ARRAY_SIZE(avr_port_lut); i++)
		if (avr_port_lut[i].pin_reg == pin_reg) // found
			break;
	if (i >= ARRAY_SIZE(avr_port_lut)) // not found
		return -1;

	// validate pin_bit
	if (pin_bit > 7)
		return -1;

	// lookup and validate arduino pin num
	for (i = 0; i < ARRAY_SIZE(pin_lut); i++)
		if (avr_port_lut[pin_lut[i].avr_port].pin_reg == pin_reg && pin_lut[i].avr_bit == pin_bit) // found
			break;
	if (i >= ARRAY_SIZE(pin_lut)) // not found
		return -1;

	// assign pin struct values
	pin->ard_pin  = i;
	pin->pin_reg  = avr_port_lut[pin_lut[i].avr_port].pin_reg;
	pin->ddr_reg  = avr_port_lut[pin_lut[i].avr_port].ddr_reg;
	pin->port_reg = avr_port_lut[pin_lut[i].avr_port].port_reg;
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
	// set pin to invalid until validated
	pin->valid_flag = PIN_INVALID;
	pin->ard_pin    = PIN_NOT_USED;

	// copy lut arrays from flash memory
	struct avr_port_lut_s avr_port_lut[ARRAY_SIZE(avr_port_lut_i)]; 
        memcpy_P(&avr_port_lut[0], &avr_port_lut_i[0], (ARRAY_SIZE(avr_port_lut)*sizeof(avr_port_lut[0])));
	struct pin_lut_s pin_lut[ARRAY_SIZE(pin_lut_i)]; 
        memcpy_P(&pin_lut[0], &pin_lut_i[0], (ARRAY_SIZE(pin_lut)*sizeof(pin_lut[0])));

	// validate pin_num
	if (pin_num >= ARRAY_SIZE(pin_lut))
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
	return (int8_t)((_SFR_IO8(pin->pin_reg) & pin->pin_mask) >> pin->pin_bit);
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
	return (int8_t)((_SFR_IO8(pin.pin_reg) & pin.pin_mask) >> pin.pin_bit);
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
