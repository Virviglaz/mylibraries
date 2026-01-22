#include "gpio.h"

int main(void)
{
    //GPIO_Device(0, 13, GPIO_DeviceBase::dir::OUTPUT).Init().Set(1);
    GPIO_Device led(2, 13, GPIO_DeviceBase::dir::OUTPUT);
    led.Init();

    while (true)
    {
        led.Set(1);
        for (volatile int i = 0; i < 1000000; ++i);
        led.Set(0);
        for (volatile int i = 0; i < 1000000; ++i);
    }


    return 0;
}
