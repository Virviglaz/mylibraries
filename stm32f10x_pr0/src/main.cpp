#include "gpio.h"
#include "timer.h"
#include "clock.h"
#include "uart.h"

static GPIO_Device led(2, 13, GPIO_DeviceBase::dir::OUTPUT);
static Timer_Device timer(2);
static UART_Device uart(1, 115200, 8000000);

void some_delay_function()
{
	for (volatile int i = 0; i < 1000000; ++i)
	{
		__asm__("nop");
	}
}

void timer_delay_function()
{
	for (volatile int i = 0; i < 100; ++i)
	{
		timer.Wait(10000);
	}
}

int main(void)
{
	Clocks::EnablePLL(9);
	Clocks::RunFromHSE();

	timer.InitAt().Enable();
	led.Init();
	uart.Init();

	while (true)
	{
		led.Set(1);
		//timer.Wait(30000);
		timer_delay_function();

		led.Set(0);
		//timer.Wait(30000);
		timer_delay_function();
	}

	return 0;
}
