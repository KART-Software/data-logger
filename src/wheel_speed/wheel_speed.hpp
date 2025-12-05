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
    void process();
    void read();

    // main.cppでシリアルモニタに表示するための「のぞき穴」関数
    // float getSpeed(uint8_t id) {
    //     switch(id) {
    //         case 0: return (float)speed0;
    //         case 1: return (float)speed1;
    //         case 2: return (float)speed2;
    //         case 3: return (float)speed3;
    //         default: return 0.0f;
    //     }
    // }

    // // デバッグ用にカウント値も見れるようにしておく
    // int16_t getCount(uint8_t id) {
    //     switch(id) {
    //         case 0: return count0;
    //         case 1: return count1;
    //         case 2: return count2;
    //         case 3: return count3;
    //         default: return 0;
    //     }
    // }

private:
    int16_t count0, count1, count2, count3; // 現在の累積カウント
    int16_t last_count0, last_count1, last_count2, last_count3; // 前回のカウント
    double speed0, speed1, speed2, speed3; // 計算した速度 (km/h)
    int16_t send_speed0, send_speed1, send_speed2, send_speed3; // 送信用整数 (km/h * 10)
    unsigned long lastGetCountTime; // 前回計算した時刻

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
    // タイヤ1
    pcnt_config_t pcnt_config1 = {
        .pulse_gpio_num = WHEELSPEED_F1_PIN,
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

    // タイヤ2
    pcnt_config_t pcnt_config2 = {
        .pulse_gpio_num = WHEELSPEED_F2_PIN,
        .ctrl_gpio_num = PCNT_PIN_NOT_USED,
        .lctrl_mode = PCNT_MODE_KEEP,
        .hctrl_mode = PCNT_MODE_KEEP,
        .pos_mode = PCNT_COUNT_INC,
        .neg_mode = PCNT_COUNT_DIS,
        .counter_h_lim = 32767,
        .counter_l_lim = -32768,
        .unit = PCNT_UNIT_2,
        .channel = PCNT_CHANNEL_0,
    };

    // タイヤ3
    pcnt_config_t pcnt_config3 = {
        .pulse_gpio_num = WHEELSPEED_F3_PIN,
        .ctrl_gpio_num = PCNT_PIN_NOT_USED,
        .lctrl_mode = PCNT_MODE_KEEP,
        .hctrl_mode = PCNT_MODE_KEEP,
        .pos_mode = PCNT_COUNT_INC,
        .neg_mode = PCNT_COUNT_DIS,
        .counter_h_lim = 32767,
        .counter_l_lim = -32768,
        .unit = PCNT_UNIT_3,
        .channel = PCNT_CHANNEL_0,
    };
};

#endif