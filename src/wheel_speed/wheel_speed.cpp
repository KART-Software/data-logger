#include "wheel_speed.hpp"

bool WheelSpeed::initialize()
{
    pcnt_unit_config(&pcnt_config0);
    pcnt_unit_config(&pcnt_config1);
    pcnt_counter_pause(PCNT_UNIT_0);
    pcnt_counter_pause(PCNT_UNIT_1);
    pcnt_counter_clear(PCNT_UNIT_0);
    pcnt_counter_clear(PCNT_UNIT_1);
    pcnt_counter_resume(PCNT_UNIT_0);
    pcnt_counter_resume(PCNT_UNIT_1);
    read();
}

void WheelSpeed::getWheelSpeed()
{
    int16_t lastcount0 = count0;
    int16_t lastcount1 = count1;
    unsigned long lasttime = lastGetCountTime;
    read();
    speed0 = (count0 - lastcount0) * PI * WHEELDIAMETER / (lastGetCountTime - lasttime) / NUM_OF_TEETH;
    speed1 = (count1 - lastcount1) * PI * WHEELDIAMETER / (lastGetCountTime - lasttime) / NUM_OF_TEETH;
}

void WheelSpeed::getAngles()
{
    angle0 = count0 * 360.0 / NUM_OF_TEETH;
    angle1 = count1 * 360.0 / NUM_OF_TEETH;
}

void WheelSpeed::getBytes(uint8_t *bytes, uint startByte)
{
    bytes[startByte] = count0;
    bytes[startByte + 2] = speed0;
    bytes[startByte + 4] = count1;
    bytes[startByte + 6] = speed1;
}

void WheelSpeed::read()
{
    lastGetCountTime = millis();
    pcnt_get_counter_value(PCNT_UNIT_0, &count0);
    pcnt_get_counter_value(PCNT_UNIT_1, &count1);
}