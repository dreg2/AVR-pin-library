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
        memcpy_P(&avr_port_lut[0], &avr_port_lut_i[0], sizeof avr_port_lut);
	struct pin_lut_s pin_lut[ARRAY_SIZE(pin_lut_i)]; 
        memcpy_P(&pin_lut[0], &pin_lut_i[0], sizeof pin_lut);

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
	pin->pin_mask = (uint8_t)(1 << pin_bit);

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
        memcpy_P(&avr_port_lut[0], &avr_port_lut_i[0], sizeof(avr_port_lut));
	struct pin_lut_s pin_lut[ARRAY_SIZE(pin_lut_i)]; 
        memcpy_P(&pin_lut[0], &pin_lut_i[0], sizeof(pin_lut));

	// validate pin_num
	if (pin_num >= ARRAY_SIZE(pin_lut))
		return -1;

	// assign pin struct values from look-up table
	pin->ard_pin  = pin_num;
	pin->pin_reg  = avr_port_lut[pin_lut[pin_num].avr_port].pin_reg;
	pin->ddr_reg  = avr_port_lut[pin_lut[pin_num].avr_port].ddr_reg;
	pin->port_reg = avr_port_lut[pin_lut[pin_num].avr_port].port_reg;
	pin->pin_bit  = pin_lut[pin_num].avr_bit;
	pin->pin_mask = (uint8_t)(1 << pin->pin_bit);

	pin->valid_flag = PIN_VALID;
	return 0;
	}

//----------------------------------------------------------------------------------------------------
// pin data direction
//----------------------------------------------------------------------------------------------------
int8_t pin_ddr(pin_t *pin, uint8_t pin_value)
	{
	// check validity
	if (pin->valid_flag != PIN_VALID)
		return -1;

	// set/clear bit in ddr register
	switch (pin_value)
		{
		case PIN_OUT:
			_SFR_IO8(pin->ddr_reg) |= pin->pin_mask;
			break;

		case PIN_IN:
			_SFR_IO8(pin->ddr_reg) &= (uint8_t)~pin->pin_mask;
			break;

		default:
			// invalid pin value
			return -1;
		}

	return 0;
	}

//----------------------------------------------------------------------------------------------------
// pin data direction from arduino pin number
//----------------------------------------------------------------------------------------------------
int8_t pin_ddr_ard(uint8_t pin_num, uint8_t pin_value)
	{
	// initialize pin struct
	pin_t pin;
	if (pin_init_ard(&pin, pin_num) < 0)
		return -1;

	// set/clear bit in ddr register
	switch (pin_value)
		{
		case PIN_OUT:
			_SFR_IO8(pin.ddr_reg) |= pin.pin_mask;
			break;

		case PIN_IN:
			_SFR_IO8(pin.ddr_reg) &= (uint8_t)~pin.pin_mask;
			break;

		default:
			// invalid pin value
			return -1;
		}

	return 0;
	}

//----------------------------------------------------------------------------------------------------
// set port pin level
//----------------------------------------------------------------------------------------------------
int8_t pin_port(pin_t *pin, uint8_t pin_value)
	{
	// check validity
	if (pin->valid_flag != PIN_VALID)
		return -1;

	// set/clear bit in port register
	switch (pin_value)
		{
		case PIN_HIGH:
			_SFR_IO8(pin->port_reg) |= pin->pin_mask;
			break;

		case PIN_LOW:
			_SFR_IO8(pin->port_reg) &= (uint8_t)~pin->pin_mask;
			break;

		default:
			// invalid pin value
			return -1;
		}

	return 0;
	}

//----------------------------------------------------------------------------------------------------
// set port pin level from arduino pin number
//----------------------------------------------------------------------------------------------------
int8_t pin_port_ard(uint8_t pin_num, uint8_t pin_value)
	{
	// initialize pin struct
	pin_t pin;
	if (pin_init_ard(&pin, pin_num) < 0)
		return -1;

	// set/clear bit in port register
	switch (pin_value)
		{
		case PIN_HIGH:
			_SFR_IO8(pin.port_reg) |= pin.pin_mask;
			break;

		case PIN_LOW:
			_SFR_IO8(pin.port_reg) &= (uint8_t)~pin.pin_mask;
			break;

		default:
			// invalid pin value
			return -1;
		}

	return 0;
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
	if (pin_init_ard(&pin, pin_num) < 0)
		return -1;

	// return bit from pin register
	return (int8_t)((_SFR_IO8(pin.pin_reg) & pin.pin_mask) >> pin.pin_bit);
	}

//----------------------------------------------------------------------------------------------------
// set pin state
//----------------------------------------------------------------------------------------------------
int8_t pin_state_set(pin_t *pin, uint8_t state)
	{
	// check validity
	if (pin->valid_flag != PIN_VALID)
		return -1;

	// set pin state
	switch (state)
		{
		case PIN_IN_HIGHZ:
			// port = 0, ddr = 0
			_SFR_IO8(pin->port_reg) &= (uint8_t)~pin->pin_mask;
			_SFR_IO8(pin->ddr_reg)  &= (uint8_t)~pin->pin_mask;
			break;

		case PIN_IN_PULLUP:
			// port = 1, ddr = 0
			_SFR_IO8(pin->port_reg) |= pin->pin_mask;
			_SFR_IO8(pin->ddr_reg)  &= (uint8_t)~pin->pin_mask;
			break;

		case PIN_OUT_LOW:
			// port = 0, ddr = 1
			_SFR_IO8(pin->port_reg) &= (uint8_t)~pin->pin_mask;
			_SFR_IO8(pin->ddr_reg)  |= pin->pin_mask;
			break;

		case PIN_OUT_HIGH:
			// port = 1, ddr = 1
			_SFR_IO8(pin->port_reg) |= pin->pin_mask;
			_SFR_IO8(pin->ddr_reg)  |= pin->pin_mask;
			break;

		default:
			// invalid state
			return -1;
		}

	return 0;
	}

//----------------------------------------------------------------------------------------------------
// set output pin level from arduino pin number
//----------------------------------------------------------------------------------------------------
int8_t pin_state_set_ard(uint8_t pin_num, uint8_t state)
	{
	// initialize pin struct
	pin_t pin;
	if (pin_init_ard(&pin, pin_num) < 0)
		return -1;

	// set pin state
	switch (state)
		{
		case PIN_IN_HIGHZ:
			// port = 0, ddr = 0
			_SFR_IO8(pin.port_reg) &= (uint8_t)~pin.pin_mask;
			_SFR_IO8(pin.ddr_reg)  &= (uint8_t)~pin.pin_mask;
			break;

		case PIN_IN_PULLUP:
			// port = 1, ddr = 0
			_SFR_IO8(pin.port_reg) |= pin.pin_mask;
			_SFR_IO8(pin.ddr_reg)  &= (uint8_t)~pin.pin_mask;
			break;

		case PIN_OUT_LOW:
			// port = 0, ddr = 1
			_SFR_IO8(pin.port_reg) &= (uint8_t)~pin.pin_mask;
			_SFR_IO8(pin.ddr_reg)  |= pin.pin_mask;
			break;

		case PIN_OUT_HIGH:
			// port = 1, ddr = 1
			_SFR_IO8(pin.port_reg) |= pin.pin_mask;
			_SFR_IO8(pin.ddr_reg)  |= pin.pin_mask;
			break;

		default:
			// invalid state
			return -1;
		}

	return 0;
	}
