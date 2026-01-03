#include "DS18B20_json.h"
#include <cassert>

int DS18B20_json_test()
{
    DS18B20_Single_Json sensor("devices_test.json", "RoomTempSensor1");
    assert(sensor.Init(0) == OneWire_InterfaceBase::Result::Success);
    assert(sensor.StartConversion() == OneWire_InterfaceBase::Result::Success);

    float temperature = 0.0f;
    assert(sensor.ReadTemperature(temperature) == OneWire_InterfaceBase::Result::Success);
    assert(temperature > 20.0f && temperature < 23.0f);
    sensor.Step(); // Simulate a step to update internal state if needed

    assert(sensor.StartConversion() == OneWire_InterfaceBase::Result::Success);
    assert(sensor.ReadTemperature(temperature) == OneWire_InterfaceBase::Result::Success);
    assert(temperature > -24.0f && temperature < -23.0f);
    sensor.Step(); // Simulate a step to update internal state if needed

    return 0;
}