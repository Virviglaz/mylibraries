#include "gpio.h"

int main(void)
{
    GPIO_Device(0, 13, GPIO_Device::OUTPUT).Init().Set(1);

    return 0;
}
