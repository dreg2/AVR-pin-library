#if defined(PIN_SFR_ASM_COMPAT)
        #define _SFR_ASM_COMPAT 1
#else
        #define _SFR_ASM_COMPAT 0
#endif
#include <avr/io.h>

#include <avr/pgmspace.h>
#include <stdio.h>

#include "common.h"
#include "pin.h"

#define PORTB_IDX 0
#define PORTC_IDX 1
#define PORTD_IDX 2

// port register pointers
#if _SFR_ASM_COMPAT
	#define PINB_PTR  ((volatile uint8_t *)PINB)
	#define DDRB_PTR  ((volatile uint8_t *)DDRB)
	#define PORTB_PTR ((volatile uint8_t *)PORTB)
	#define PINC_PTR  ((volatile uint8_t *)PINC)
	#define DDRC_PTR  ((volatile uint8_t *)DDRC)
	#define PORTC_PTR ((volatile uint8_t *)PORTC)
	#define PIND_PTR  ((volatile uint8_t *)PIND)
	#define DDRD_PTR  ((volatile uint8_t *)DDRD)
	#define PORTD_PTR ((volatile uint8_t *)PORTD)
#else
	// atmega328p values (other models may be different)
	#define PINB_PTR  ((volatile uint8_t *)0x03)
	#define DDRB_PTR  ((volatile uint8_t *)0x04)
	#define PORTB_PTR ((volatile uint8_t *)0x05)
	#define PINC_PTR  ((volatile uint8_t *)0x06)
	#define DDRC_PTR  ((volatile uint8_t *)0x07)
	#define PORTC_PTR ((volatile uint8_t *)0x08)
	#define PIND_PTR  ((volatile uint8_t *)0x09)
	#define DDRD_PTR  ((volatile uint8_t *)0x0A)
	#define PORTD_PTR ((volatile uint8_t *)0x0B)
#endif

// avr pin port register pointers look-up table
const struct avr_port_lut_s
        {
        volatile uint8_t *pin_reg;   // pinx register pointer
        volatile uint8_t *ddr_reg;   // ddrx register pointer
        volatile uint8_t *port_reg;  // portx register pointer
        } PROGMEM avr_port_lut_i[] =
		{
		{PINB_PTR, DDRB_PTR, PORTB_PTR},
		{PINC_PTR, DDRC_PTR, PORTC_PTR},
		{PIND_PTR, DDRD_PTR, PORTD_PTR}
		};

// pin look up table - arduino pin number is the index
const struct pin_lut_s
	{
        uint8_t   avr_port_idx;  // avr pin port lut index
	uint8_t   avr_bit;       // avr bit position
	} PROGMEM pin_lut_i[] =
		{
		{PORTD_IDX, PIND0},
		{PORTD_IDX, PIND1},
		{PORTD_IDX, PIND2},
		{PORTD_IDX, PIND3},
		{PORTD_IDX, PIND4},
		{PORTD_IDX, PIND5},
		{PORTD_IDX, PIND6},
		{PORTD_IDX, PIND7},
		{PORTB_IDX, PINB0},
		{PORTB_IDX, PINB1},
		{PORTB_IDX, PINB2},
		{PORTB_IDX, PINB3},
		{PORTB_IDX, PINB4},
		{PORTB_IDX, PINB5},
		{PORTC_IDX, PINC0},
		{PORTC_IDX, PINC1},
		{PORTC_IDX, PINC2},
		{PORTC_IDX, PINC3},
		{PORTC_IDX, PINC4},
		{PORTC_IDX, PINC5},
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
		if (avr_port_lut[pin_lut[i].avr_port_idx].pin_reg == pin_reg && pin_lut[i].avr_bit == pin_bit) // found
			break;
	if (i >= ARRAY_SIZE(pin_lut)) // not found
		return -1;

	// assign pin struct values
	pin->ard_pin  = i;
	pin->pin_reg  = avr_port_lut[pin_lut[i].avr_port_idx].pin_reg;
	pin->ddr_reg  = avr_port_lut[pin_lut[i].avr_port_idx].ddr_reg;
	pin->port_reg = avr_port_lut[pin_lut[i].avr_port_idx].port_reg;
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
        memcpy_P(&avr_port_lut[0], &avr_port_lut_i[0], sizeof avr_port_lut);
	struct pin_lut_s pin_lut[ARRAY_SIZE(pin_lut_i)]; 
        memcpy_P(&pin_lut[0], &pin_lut_i[0], sizeof pin_lut);

	// validate pin_num
	if (pin_num >= ARRAY_SIZE(pin_lut))
		return -1;

	// assign pin struct values from look-up table
	pin->ard_pin  = pin_num;
	pin->pin_reg  = avr_port_lut[pin_lut[pin_num].avr_port_idx].pin_reg;
	pin->ddr_reg  = avr_port_lut[pin_lut[pin_num].avr_port_idx].ddr_reg;
	pin->port_reg = avr_port_lut[pin_lut[pin_num].avr_port_idx].port_reg;
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
#if _SFR_ASM_COMPAT
			*pin->ddr_reg |= pin->pin_mask;
#else
			_SFR_IO8(pin->ddr_reg) |= pin->pin_mask;
#endif
			break;

		case PIN_IN:
#if _SFR_ASM_COMPAT
			*pin->ddr_reg &= (uint8_t)~pin->pin_mask;
#else
			_SFR_IO8(pin->ddr_reg) &= (uint8_t)~pin->pin_mask;
#endif
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
#if _SFR_ASM_COMPAT
			*pin.ddr_reg |= pin.pin_mask;
#else
			_SFR_IO8(pin.ddr_reg) |= pin.pin_mask;
#endif
			break;

		case PIN_IN:
#if _SFR_ASM_COMPAT
			*pin.ddr_reg &= (uint8_t)~pin.pin_mask;
#else
			_SFR_IO8(pin.ddr_reg) &= (uint8_t)~pin.pin_mask;
#endif
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
#if _SFR_ASM_COMPAT
			*pin->port_reg |= pin->pin_mask;
#else
			_SFR_IO8(pin->port_reg) |= pin->pin_mask;
#endif
			break;

		case PIN_LOW:
#if _SFR_ASM_COMPAT
			*pin->port_reg &= (uint8_t)~pin->pin_mask;
#else
			_SFR_IO8(pin->port_reg) &= (uint8_t)~pin->pin_mask;
#endif
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
#if _SFR_ASM_COMPAT
			*pin.port_reg |= pin.pin_mask;
#else
			_SFR_IO8(pin.port_reg) |= pin.pin_mask;
#endif
			break;

		case PIN_LOW:
#if _SFR_ASM_COMPAT
			*pin.port_reg &= (uint8_t)~pin.pin_mask;
#else
			_SFR_IO8(pin.port_reg) &= (uint8_t)~pin.pin_mask;
#endif
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
#if _SFR_ASM_COMPAT
	return (int8_t)((*pin->pin_reg & pin->pin_mask) >> pin->pin_bit);
#else
	return (int8_t)((_SFR_IO8(pin->pin_reg) & pin->pin_mask) >> pin->pin_bit);
#endif
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
#if _SFR_ASM_COMPAT
	return (int8_t)((*pin.pin_reg & pin.pin_mask) >> pin.pin_bit);
#else
	return (int8_t)((_SFR_IO8(pin.pin_reg) & pin.pin_mask) >> pin.pin_bit);
#endif
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
#if _SFR_ASM_COMPAT
			*pin->port_reg &= (uint8_t)~pin->pin_mask;
			*pin->ddr_reg  &= (uint8_t)~pin->pin_mask;
#else
			_SFR_IO8(pin->port_reg) &= (uint8_t)~pin->pin_mask;
			_SFR_IO8(pin->ddr_reg)  &= (uint8_t)~pin->pin_mask;
#endif
			break;

		case PIN_IN_PULLUP:
			// port = 1, ddr = 0
#if _SFR_ASM_COMPAT
			*pin->port_reg |= pin->pin_mask;
			*pin->ddr_reg  &= (uint8_t)~pin->pin_mask;
#else
			_SFR_IO8(pin->port_reg) |= pin->pin_mask;
			_SFR_IO8(pin->ddr_reg)  &= (uint8_t)~pin->pin_mask;
#endif
			break;

		case PIN_OUT_LOW:
			// port = 0, ddr = 1
#if _SFR_ASM_COMPAT
			*pin->port_reg &= (uint8_t)~pin->pin_mask;
			*pin->ddr_reg  |= pin->pin_mask;
#else
			_SFR_IO8(pin->port_reg) &= (uint8_t)~pin->pin_mask;
			_SFR_IO8(pin->ddr_reg)  |= pin->pin_mask;
#endif
			break;

		case PIN_OUT_HIGH:
			// port = 1, ddr = 1
#if _SFR_ASM_COMPAT
			*pin->port_reg |= pin->pin_mask;
			*pin->ddr_reg  |= pin->pin_mask;
#else
			_SFR_IO8(pin->port_reg) |= pin->pin_mask;
			_SFR_IO8(pin->ddr_reg)  |= pin->pin_mask;
#endif
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
#if _SFR_ASM_COMPAT
			*pin.port_reg &= (uint8_t)~pin.pin_mask;
			*pin.ddr_reg  &= (uint8_t)~pin.pin_mask;
#else
			_SFR_IO8(pin.port_reg) &= (uint8_t)~pin.pin_mask;
			_SFR_IO8(pin.ddr_reg)  &= (uint8_t)~pin.pin_mask;
#endif
			break;

		case PIN_IN_PULLUP:
			// port = 1, ddr = 0
#if _SFR_ASM_COMPAT
			*pin.port_reg |= pin.pin_mask;
			*pin.ddr_reg  &= (uint8_t)~pin.pin_mask;
#else
			_SFR_IO8(pin.port_reg) |= pin.pin_mask;
			_SFR_IO8(pin.ddr_reg)  &= (uint8_t)~pin.pin_mask;
#endif
			break;

		case PIN_OUT_LOW:
			// port = 0, ddr = 1
#if _SFR_ASM_COMPAT
			*pin.port_reg &= (uint8_t)~pin.pin_mask;
			*pin.ddr_reg  |= pin.pin_mask;
#else
			_SFR_IO8(pin.port_reg) &= (uint8_t)~pin.pin_mask;
			_SFR_IO8(pin.ddr_reg)  |= pin.pin_mask;
#endif
			break;

		case PIN_OUT_HIGH:
			// port = 1, ddr = 1
#if _SFR_ASM_COMPAT
			*pin.port_reg |= pin.pin_mask;
			*pin.ddr_reg  |= pin.pin_mask;
#else
			_SFR_IO8(pin.port_reg) |= pin.pin_mask;
			_SFR_IO8(pin.ddr_reg)  |= pin.pin_mask;
#endif
			break;

		default:
			// invalid state
			return -1;
		}

	return 0;
	}
