#include "gpio.h"
#include "timer.h"
#include "clock.h"
#include "uart.h"
#include "delays.h"

static GPIO_Device led(GPIOC, 13, GPIO_Output());
static UART_Device uart1(UART_Device::UartDevice::UART_1, 115200);

/*
./JLinkGDBServer -select USB -device STM32F103C8 -endian little -if SWD -speed auto -ir -noLocalhostOnly -nologtofile -port 2331 -SWOPort 2332 -TelnetPort 2333
*/

int main(void)
{
	//SystemClock::Configure();

	led.Init();
	uart1.Init();
	uart1.UseForPrintf();

	while (true)
	{
		led.Set(1);
		//Delay::ms(500);

		led.Set(0);
		//Delay::ms(500);
	}

	return 0;
}
