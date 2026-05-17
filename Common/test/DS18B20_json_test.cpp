#include "DS18B20_json.h"
#include <gtest/gtest.h>

int DS18B20_json_test()
{
    DS18B20_Single_Json sensor("devices_test.json", "RoomTempSensor1");
    EXPECT_EQ(sensor.Init(0), OneWire_InterfaceBase::Result::Success);
    EXPECT_EQ(sensor.StartConversion(), OneWire_InterfaceBase::Result::Success);

    float temperature = 0.0f;
    EXPECT_EQ(sensor.ReadTemperature(temperature), OneWire_InterfaceBase::Result::Success);
    EXPECT_GT(temperature, 20.0f);
    EXPECT_LT(temperature, 23.0f);
    sensor.Step(); // Simulate a step to update internal state if needed

    EXPECT_EQ(sensor.StartConversion(), OneWire_InterfaceBase::Result::Success);
    EXPECT_EQ(sensor.ReadTemperature(temperature), OneWire_InterfaceBase::Result::Success);
    EXPECT_GT(temperature, -24.0f);
    EXPECT_LT(temperature, -23.0f);
    sensor.Step(); // Simulate a step to update internal state if needed

    return 0;
}