#include "wheel_speed.hpp"
#include <math.h> // round関数などを使うため

bool WheelSpeed::initialize()
{
    pcnt_unit_config(&pcnt_config0);
    pcnt_unit_config(&pcnt_config1);
    pcnt_unit_config(&pcnt_config2);
    pcnt_unit_config(&pcnt_config3);

    // 2. ノイズフィルタの設定 
    // 100クロック(約1.25us)未満の短いパルスノイズを無視します
    // ノイズが多い場合は数値を増やしてください (最大1023)
    int16_t filter_val = 100;
    pcnt_set_filter_value(PCNT_UNIT_0, filter_val); pcnt_filter_enable(PCNT_UNIT_0);
    pcnt_set_filter_value(PCNT_UNIT_1, filter_val); pcnt_filter_enable(PCNT_UNIT_1);
    pcnt_set_filter_value(PCNT_UNIT_2, filter_val); pcnt_filter_enable(PCNT_UNIT_2);
    pcnt_set_filter_value(PCNT_UNIT_3, filter_val); pcnt_filter_enable(PCNT_UNIT_3);

    pcnt_counter_pause(PCNT_UNIT_0);// 一旦停止
    pcnt_counter_pause(PCNT_UNIT_1);
    pcnt_counter_pause(PCNT_UNIT_2);
    pcnt_counter_pause(PCNT_UNIT_3);
    pcnt_counter_clear(PCNT_UNIT_0);// カウント値をゼロに戻す
    pcnt_counter_clear(PCNT_UNIT_1);
    pcnt_counter_clear(PCNT_UNIT_2);
    pcnt_counter_clear(PCNT_UNIT_3);
    pcnt_counter_resume(PCNT_UNIT_0);// 計測再開
    pcnt_counter_resume(PCNT_UNIT_1);
    pcnt_counter_resume(PCNT_UNIT_2);
    pcnt_counter_resume(PCNT_UNIT_3);

    read();

    last_count0 = count0;
    last_count1 = count1;
    last_count2 = count2;
    last_count3 = count3;

    return true;
}

void WheelSpeed::read()
{
    lastGetCountTime = millis();
    pcnt_get_counter_value(PCNT_UNIT_0, &count0);
    pcnt_get_counter_value(PCNT_UNIT_1, &count1);
    pcnt_get_counter_value(PCNT_UNIT_2, &count2);
    pcnt_get_counter_value(PCNT_UNIT_3, &count3);
}

void WheelSpeed::process()
{
    //最新の値を読む(count変数とlastGetCountTimeが更新される)
    read();
    
    //パルスあたりの角度 [度]
    double deg_per_pulse = 360.0 / NUM_OF_TEETH;

    //差分計算 (前回から何パルス進んだか)
    int16_t diff0 = count0 - last_count0;
    int16_t diff1 = count1 - last_count1;
    int16_t diff2 = count2 - last_count2;
    int16_t diff3 = count3 - last_count3;

    //今の値を「前回」として保存 (次回の計算用)
    last_count0 = count0;
    last_count1 = count1;
    last_count2 = count2;
    last_count3 = count3;

    //角度に変換 [度]
    delta_angle0 = (double)diff0 * deg_per_pulse;
    delta_angle1 = (double)diff1 * deg_per_pulse;
    delta_angle2 = (double)diff2 * deg_per_pulse;
    delta_angle3 = (double)diff3 * deg_per_pulse;
    
}

void WheelSpeed::getBytes(uint8_t *bytes, uint startByte)
{
    //回転角度を 10倍して整数化 (0.1度単位)
    send_angle0 = (int16_t)round(delta_angle0 * 10.0);
    send_angle1 = (int16_t)round(delta_angle1 * 10.0);
    send_angle2 = (int16_t)round(delta_angle2 * 10.0);
    send_angle3 = (int16_t)round(delta_angle3 * 10.0);

    // バイト配列に格納 (合計16バイト)
    // [Rawカウント(2B)] + [回転角度(2B)] のセット
    // --- タイヤ0 ---
    bytes[startByte + 0] = (count0 >> 8) & 0xFF;
    bytes[startByte + 1] = count0 & 0xFF;
    bytes[startByte + 2] = (send_angle0 >> 8) & 0xFF;
    bytes[startByte + 3] = send_angle0 & 0xFF;

    // --- タイヤ1 ---
    bytes[startByte + 4] = (count1 >> 8) & 0xFF;
    bytes[startByte + 5] = count1 & 0xFF;
    bytes[startByte + 6] = (send_angle1 >> 8) & 0xFF;
    bytes[startByte + 7] = send_angle1 & 0xFF;

    // --- タイヤ2 ---
    bytes[startByte + 8] = (count2 >> 8) & 0xFF;
    bytes[startByte + 9] = count2 & 0xFF;
    bytes[startByte + 10] = (send_angle2 >> 8) & 0xFF;
    bytes[startByte + 11] = send_angle2 & 0xFF;

    // --- タイヤ3 ---
    bytes[startByte + 12] = (count3 >> 8) & 0xFF;
    bytes[startByte + 13] = count3 & 0xFF;
    bytes[startByte + 14] = (send_angle3 >> 8) & 0xFF;
    bytes[startByte + 15] = send_angle3 & 0xFF;
}

