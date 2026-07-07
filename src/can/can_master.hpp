#ifndef _CAN_MASTER_H_
#define _CAN_MASTER_H_

#include "can.hpp"
#include "spi/bmi160.hpp"
#include "spi/ads8688.hpp"
#include "gps/gps.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "util/toggle_switch.hpp"

#define BMI160_DATA_LENGTH 12
#define ADS8688_DATA_LENGTH 16
#define GPS_DATA_LENGTH 92

#define CAN_DATA_LENGTH (BMI160_DATA_LENGTH + ADS8688_DATA_LENGTH + GPS_DATA_LENGTH) // 120
#define CAN_NUM_MESSAGES 15

// KartControl(0x740) byte0 = ETC mode の値。ID/レイアウトは kart-can (生成 kart.h) を使用。
enum CanControlMode : uint8_t
{
    CTRL_MODE_CALIB = 1,
    CTRL_MODE_NORMAL = 2,
    CTRL_MODE_RESTRICTED = 3,
    CTRL_MODE_MOTOR_OFF = 4,
};

class CanMaster
{
public:
    CanMaster(Bmi160 &bmi160, Ads8688 &ads8688, GPS &gps,
              SelectSwitch3Pin &modeSwitch, ToggleSwitch &launchSwitch, ToggleSwitch &autoShiftSwitch);
    esp_err_t initialize();
    esp_err_t send();
    void run();
    // 入力(センサ/スイッチ)保護用ミューテックス。loop()(別コア)のサンプリングと
    // CAN タスクの状態読み出しを排他するため共有する。
    SemaphoreHandle_t getSensorMutex() { return sensorMutex; }

private:
    CanBus bus = CanBus();
    Bmi160 &bmi160;
    Ads8688 &ads8688;
    GPS &gps;
    SemaphoreHandle_t sensorMutex = nullptr;
    uint8_t data[CAN_DATA_LENGTH] = {};  // センサ生バイトを集約する120Bバッファ (GPS getBytes失敗時は前回値保持)

    // 制御スイッチ (所有は main。サンプリングは loop() で、ここでは状態を読むだけ)。
    SelectSwitch3Pin &modeSwitch;
    ToggleSwitch &launchSwitch;
    ToggleSwitch &autoShiftSwitch;

    void getData();           // センサ生バイトを 120B バッファ data[] に集約
    esp_err_t sendControl();  // スイッチ状態を Control(0x740) として pack 送信
};

void startCan(void *canMaster);

#endif