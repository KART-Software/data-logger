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
    unsigned long prev_time = lastGetCountTime;
    read(); 
    unsigned long current_time = lastGetCountTime;

    //経過時間 [ms]
    unsigned long dt_ms = current_time - prev_time;

    // ゼロ除算防止（dtが0なら計算しない）
    if (dt_ms == 0) return;

    // 3. 時間を「秒」に変換
    float time_sec = (float)dt_ms / 1000.0;
    
    float dist_per_pulse = (WHEELDIAMETER * M_PI) / NUM_OF_TEETH;

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

    //速度計算: (距離 / 時間) * 3.6
    speed0 = ((float)diff0 * dist_per_pulse / time_sec) * 3.6;
    speed1 = ((float)diff1 * dist_per_pulse / time_sec) * 3.6;
    speed2 = ((float)diff2 * dist_per_pulse / time_sec) * 3.6;
    speed3 = ((float)diff3 * dist_per_pulse / time_sec) * 3.6;
    
}

void WheelSpeed::getBytes(uint8_t *bytes, uint startByte)
{
    //時速を 10倍して整数化 (0.1度単位)
    send_speed0 = (int16_t)round(speed0 * 10.0);
    send_speed1 = (int16_t)round(speed1 * 10.0);
    send_speed2 = (int16_t)round(speed2 * 10.0);
    send_speed3 = (int16_t)round(speed3 * 10.0);

    // バイト配列に格納 (合計16バイト)    
    bytes[startByte + 0] = (send_speed0 >> 8) & 0xFF;
    bytes[startByte + 1] = send_speed0 & 0xFF;
    bytes[startByte + 2] = (send_speed1 >> 8) & 0xFF;
    bytes[startByte + 3] = send_speed1 & 0xFF;
    bytes[startByte + 4] = (send_speed2 >> 8) & 0xFF;
    bytes[startByte + 5] = send_speed2 & 0xFF;
    bytes[startByte + 6] = (send_speed3 >> 8) & 0xFF;
    bytes[startByte + 7] = send_speed3 & 0xFF;
    
    bytes[startByte + 8] = (count0 >> 8) & 0xFF;
    bytes[startByte + 9] = count0 & 0xFF;
    bytes[startByte + 10] = (count1 >> 8) & 0xFF;
    bytes[startByte + 11] = count1 & 0xFF;
    bytes[startByte + 12] = (count2 >> 8) & 0xFF;
    bytes[startByte + 13] = count2 & 0xFF;
    bytes[startByte + 14] = (count3 >> 8) & 0xFF;
    bytes[startByte + 15] = count3 & 0xFF;
}
   

