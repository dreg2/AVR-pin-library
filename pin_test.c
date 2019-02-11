#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "uart.h"
#include "pin.h"

int main(void)
	{
        uart_init_115200();           // initialize uart
        printf("uart initialized\n");
        getchar();

	pin_t pin_led; // test pin
	
	pin_init(&pin_led, PIN_B, PINB5); // PB5 = led
	printf("pin test 1: %p %p %p 0x%02hx 0x%02hx\n",
		pin_led.avr_pin.pin_reg, pin_led.avr_pin.ddr_reg, pin_led.avr_pin.port_reg, pin_led.avr_pin.pin_bit, pin_led.avr_pin.pin_mask);
        getchar();

	pin_ddr(&pin_led, PIN_OUT);
	for (int i = 0; i < 20; i++)
		{
		// directly access port reg
		_SFR_IO8(pin_led.avr_pin.port_reg) ^= pin_led.avr_pin.pin_mask;
		_delay_ms(200);
		}

	pin_init_ard(&pin_led, 13); // 13 = led
	printf("pin test 2: %p %p %p 0x%02hx 0x%02hx\n",
		pin_led.avr_pin.pin_reg, pin_led.avr_pin.ddr_reg, pin_led.avr_pin.port_reg, pin_led.avr_pin.pin_bit, pin_led.avr_pin.pin_mask);
        getchar();

	pin_ddr(&pin_led, PIN_OUT);
	for (int i = 0; i < 10; i++)
		{
		// use lib function to access port reg
		pin_out(&pin_led, PIN_HIGH);
		_delay_ms(200);
		pin_out(&pin_led, PIN_LOW);
		_delay_ms(200);
		}

	printf("end program\n");
	return 0;
	}

