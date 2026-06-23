#include "gpio.h"
#include "timer.h"
#include "clock.h"
#include "uart.h"
#include "delays.h"

static GPIO_Device led(2, 13, GPIO_DeviceBase::Direction::OUTPUT);
static Timer_Device timer(2);
static UART_Device uart(1, 115200, 8000000);

/*
./JLinkGDBServer -select USB -device STM32F103C8 -endian little -if SWD -speed auto -ir -noLocalhostOnly -nologtofile -port 2331 -SWOPort 2332 -TelnetPort 2333
*/

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
	//Clocks::RunFromHSE();
	//Clocks::EnablePLL(9);

	//timer.InitAt().Enable();
	led.Init();
	//uart.Init();
	Delays::Init();

	while (true)
	{
		led.Set(1);
		//Delays::DelayMs(500);
		//timer.Wait(30000);
		//timer_delay_function();
		some_delay_function();

		led.Set(0);
		//Delays::DelayMs(500);
		//timer.Wait(30000);
		//timer_delay_function();
		some_delay_function();
	}

	return 0;
}
