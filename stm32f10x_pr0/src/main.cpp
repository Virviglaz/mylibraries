#include "gpio.h"
#include "timer.h"
#include "clock.h"
#include "uart.h"

static GPIO_Device led(2, 13, GPIO_DeviceBase::dir::OUTPUT);
static Timer_Device timer(2);
static UART_Device uart(1, 115200, 8000000);

int main(void)
{
	Clocks::RunFromHSE();

	timer.InitAt().Enable();
	led.Init();
	uart.Init();

	while (true)
	{
		led.Set(1);
		timer.Wait(50000);

		led.Set(0);
		timer.Wait(50000);
	}

	return 0;
}
