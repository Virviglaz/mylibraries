#include "gpio.h"
#include "timer.h"
#include "clock.h"

static GPIO_Device led(2, 13, GPIO_DeviceBase::dir::OUTPUT);
static Timer_Device timer(2);

int main(void)
{
	Clocks::RunFromHSE(8000000UL);

	timer.InitAt().Enable();
	led.Init();

	while (true)
	{
		led.Set(1);
		timer.Wait(50000);

		led.Set(0);
		timer.Wait(50000);
	}

	return 0;
}
