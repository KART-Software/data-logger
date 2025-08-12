#ifndef _WHEEL_SPEED_HPP_
#define _WHEEL_SPEED_HPP_

#include <driver/pcnt.h>
#include "config.hpp"
#include <Arduino.h>

class WheelSpeed
{
public:
    bool initialize();
    void getBytes(uint8_t *bytes, uint startByte);
    void getWheelSpeed();
    void getAngles();
    void read();

private:
    int16_t count0, count1;
    double speed0, speed1;
    int16_t angle0, angle1;
    unsigned long lastGetCountTime;
    pcnt_config_t pcnt_config0 =
        {
            .pulse_gpio_num = WHEELSPEED_F0_PIN,
            .ctrl_gpio_num = PCNT_PIN_NOT_USED,
            .lctrl_mode = PCNT_MODE_KEEP,
            .hctrl_mode = PCNT_MODE_KEEP,
            .pos_mode = PCNT_COUNT_INC,
            .neg_mode = PCNT_COUNT_DIS,
            .counter_h_lim = 32767,
            .counter_l_lim = -32768,
            .unit = PCNT_UNIT_0,
            .channel = PCNT_CHANNEL_0,
    };
    pcnt_config_t pcnt_config1 =
        {
            .pulse_gpio_num = WHEELSPEED_F0_PIN,
            .ctrl_gpio_num = PCNT_PIN_NOT_USED,
            .lctrl_mode = PCNT_MODE_KEEP,
            .hctrl_mode = PCNT_MODE_KEEP,
            .pos_mode = PCNT_COUNT_INC,
            .neg_mode = PCNT_COUNT_DIS,
            .counter_h_lim = 32767,
            .counter_l_lim = -32768,
            .unit = PCNT_UNIT_1,
            .channel = PCNT_CHANNEL_0,
    };
};

#endif