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

	pin_t pin_test; // test pin

	// highz test
	pin_init_ard(&pin_test, 3);
	pin_state_set(&pin_test, PIN_IN_HIGHZ);
        printf("pin 3 highz\n");
        getchar();

	// pullup test
	pin_init_ard(&pin_test, 3);
	pin_state_set(&pin_test, PIN_IN_PULLUP);
        printf("pin 3 pullup\n");
        getchar();

	// out low test
	pin_init_ard(&pin_test, 3);
	pin_state_set(&pin_test, PIN_OUT_LOW);
        printf("pin 3 out low\n");
        getchar();

	// out high test
	pin_init_ard(&pin_test, 3);
	pin_state_set(&pin_test, PIN_OUT_HIGH);
        printf("pin 3 out high\n");
        getchar();

	// pin init test
	pin_init(&pin_test, PIN_B_PTR, PINB5); // PB5 = led
	printf("pin test 1: %d %p %p %p %d 0x%02hx\n",
		pin_test.ard_pin, pin_test.pin_reg, pin_test.ddr_reg, pin_test.port_reg, pin_test.pin_bit, pin_test.pin_mask);
        getchar();

	pin_ddr(&pin_test, PIN_OUT);
	for (int i = 0; i < 20; i++)
		{
		// directly access port reg
		_SFR_IO8(pin_test.port_reg) ^= pin_test.pin_mask;
		_delay_ms(200);
		}

	// pin init arduino test
//	pin_init_ard(&pin_test, 13); // 13 = led
	pin_init_ard(&pin_test, PIN_B5_ARD); // B5 = led
	printf("pin test 2: %d %p %p %p %d 0x%02hx\n",
		pin_test.ard_pin, pin_test.pin_reg, pin_test.ddr_reg, pin_test.port_reg, pin_test.pin_bit, pin_test.pin_mask);
        getchar();

	pin_ddr(&pin_test, PIN_OUT);
	for (int i = 0; i < 10; i++)
		{
		// use lib function to access port reg
		pin_state_set(&pin_test, PIN_OUT_HIGH);
		_delay_ms(200);
		pin_state_set(&pin_test, PIN_OUT_LOW);
		_delay_ms(200);
		}

	// pin in test
	pin_init(&pin_test, PIN_B_PTR, PINB1); // PB1
	printf("pin in test 1: %d %p %p %p %d 0x%02hx\n",
		pin_test.ard_pin, pin_test.pin_reg, pin_test.ddr_reg, pin_test.port_reg, pin_test.pin_bit, pin_test.pin_mask);
        getchar();
	pin_state_set(&pin_test, PIN_IN_PULLUP);
	int8_t pin_test_value;
	pin_test_value = pin_in(&pin_test);
	printf("pin in 1 = 0x%02hx\n", pin_test_value);
        getchar();
	pin_test_value = pin_in(&pin_test);
	printf("pin in 2 = 0x%02hx\n", pin_test_value);
        getchar();
	

	printf("end program\n");
	return 0;
	}

