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
	

	pin_ddr_ard(3, PIN_IN);
	pin_pu_ard(3, PIN_PULLUP_DISABLE);
        printf("pin 3 pu disabled\n");
        getchar();

	pin_pu_ard(3, PIN_PULLUP_ENABLE);
        printf("pin 3 pu enabled\n");
        getchar();

	pin_pu_ard(3, PIN_PULLUP_DISABLE);
        printf("pin 3 pu disabled\n");
        getchar();

	pin_ddr_ard(3, PIN_OUT);
	pin_out_ard(3, PIN_LOW);
        printf("pin 3 output low\n");
        getchar();

	pin_out_ard(3, PIN_HIGH);
        printf("pin 3 output high\n");
        getchar();

	pin_out_ard(3, PIN_LOW);
        printf("pin 3 output low\n");
        getchar();

	pin_init(&pin_led, PIN_B_PTR, PINB5); // PB5 = led
	printf("pin test 1: %d %p %p %p %d 0x%02hx\n",
		pin_led.ard_pin, pin_led.pin_reg, pin_led.ddr_reg, pin_led.port_reg, pin_led.pin_bit, pin_led.pin_mask);
        getchar();

	pin_ddr(&pin_led, PIN_OUT);
	for (int i = 0; i < 20; i++)
		{
		// directly access port reg
		_SFR_IO8(pin_led.port_reg) ^= pin_led.pin_mask;
		_delay_ms(200);
		}

//	pin_init_ard(&pin_led, 13); // 13 = led
	pin_init_ard(&pin_led, PIN_B5_ARD); // B5 = led
	printf("pin test 2: %d %p %p %p %d 0x%02hx\n",
		pin_led.ard_pin, pin_led.pin_reg, pin_led.ddr_reg, pin_led.port_reg, pin_led.pin_bit, pin_led.pin_mask);
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

