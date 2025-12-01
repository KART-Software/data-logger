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

void WheelSpeed::getWheelSpeed()
{
    // 1. 前回の値を保存
    int16_t lc0 = count0;
    int16_t lc1 = count1;
    int16_t lc2 = count2;
    int16_t lc3 = count3;
    unsigned long lasttime = lastGetCountTime;

    // 2. 最新の値を読む(count変数とlastGetCountTimeが更新される)
    read();
    
    // 3. 経過時間を計算 [ms]
    unsigned long dt_ms = lastGetCountTime - lasttime;

    // ゼロ除算防止（dtが0なら計算しない）
    if (dt_ms == 0) return;

    // 時間を秒に変換
    float time_sec = (float)dt_ms / 1000.0;

    // 共通の係数を計算 ( 円周[m] / 歯数 )
    float dist_per_pulse = (WHEELDIAMETER * M_PI) / NUM_OF_TEETH;

    // --- タイヤ0 (Unit 0) ---
    // パルス差分 (オーバーフローしてもint16同士の引き算なら正常に差分が出ます)
    int16_t diff0 = count0 - lc0; 
    float dist0 = (float)diff0 * dist_per_pulse; // 進んだ距離[m]
    speed0 = (dist0 / time_sec) * 3.6;           // m/s -> km/h
    
    // --- タイヤ1 (Unit 1) ---
    int16_t diff1 = count1 - lc1;
    float dist1 = (float)diff1 * dist_per_pulse;
    speed1 = (dist1 / time_sec) * 3.6;

    // --- タイヤ2 (Unit 2) ---
    int16_t diff2 = count2 - lc2;
    float dist2 = (float)diff2 * dist_per_pulse;
    speed2 = (dist2 / time_sec) * 3.6;

    // --- タイヤ3 (Unit 3) ---
    int16_t diff3 = count3 - lc3;
    float dist3 = (float)diff3 * dist_per_pulse;
    speed3 = (dist3 / time_sec) * 3.6;
    
}

void WheelSpeed::getAngles()
{
    float deg_per_pulse = 360.0 / NUM_OF_TEETH;
    
    angle0 = (int16_t)(count0 * deg_per_pulse);
    angle1 = (int16_t)(count1 * deg_per_pulse);
    angle2 = (int16_t)(count2 * deg_per_pulse);
    angle3 = (int16_t)(count3 * deg_per_pulse);
}

void WheelSpeed::getBytes(uint8_t *bytes, uint startByte)
{
    // km/h を 10倍して整数化 (例: 50.5km/h -> 505)
    processed_speed0 = (int16_t)round(speed0 * 10);
    processed_speed1 = (int16_t)round(speed1 * 10);
    processed_speed2 = (int16_t)round(speed2 * 10);
    processed_speed3 = (int16_t)round(speed3 * 10);

    // バイト配列に格納 (ビッグエンディアン: 上位バイト→下位バイト)
    // 合計 16バイト (4輪 × 4バイト) を書き込みます

    // --- タイヤ0 (offset +0) ---
    bytes[startByte + 0] = (count0 >> 8) & 0xFF;           // Count High
    bytes[startByte + 1] = count0 & 0xFF;                  // Count Low
    bytes[startByte + 2] = (processed_speed0 >> 8) & 0xFF; // Speed High
    bytes[startByte + 3] = processed_speed0 & 0xFF;        // Speed Low

    // --- タイヤ1 (offset +4) ---
    bytes[startByte + 4] = (count1 >> 8) & 0xFF;
    bytes[startByte + 5] = count1 & 0xFF;
    bytes[startByte + 6] = (processed_speed1 >> 8) & 0xFF;
    bytes[startByte + 7] = processed_speed1 & 0xFF;

    // --- タイヤ2 (offset +8) ---
    bytes[startByte + 8] = (count2 >> 8) & 0xFF;
    bytes[startByte + 9] = count2 & 0xFF;
    bytes[startByte + 10] = (processed_speed2 >> 8) & 0xFF;
    bytes[startByte + 11] = processed_speed2 & 0xFF;

    // --- タイヤ3 (offset +12) ---
    bytes[startByte + 12] = (count3 >> 8) & 0xFF;
    bytes[startByte + 13] = count3 & 0xFF;
    bytes[startByte + 14] = (processed_speed3 >> 8) & 0xFF;
    bytes[startByte + 15] = processed_speed3 & 0xFF;
}

