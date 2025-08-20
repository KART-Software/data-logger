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
    processed_speed0 = round(speed0 * 10);
    processed_speed1 = round(speed1 * 10);
    bytes[startByte] = (count0 >> 8) & 0xFF;
    bytes[startByte + 1] = count0 & 0xFF;
    bytes[startByte + 2] = (processed_speed0 >> 8) & 0xFF;
    bytes[startByte + 3] = processed_speed0 & 0xFF;
    bytes[startByte + 4] = (count1 >> 8) & 0xFF;
    bytes[startByte + 5] = count1 & 0xFF;
    bytes[startByte + 6] = (processed_speed1 >> 8) & 0xFF;
    bytes[startByte + 7] = processed_speed1 & 0xFF;
}

void WheelSpeed::read()
{
    lastGetCountTime = millis();
    pcnt_get_counter_value(PCNT_UNIT_0, &count0);
    pcnt_get_counter_value(PCNT_UNIT_1, &count1);
}