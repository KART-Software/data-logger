#ifndef _CAN_MASTER_H_
#define _CAN_MASTER_H_

#include "can.hpp"
#include "spi/bmi160.hpp"
#include "spi/ads8688.hpp"
#include "gps/gps.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "util/toggle_switch.hpp"

#define CAN_ID_START 0x700

#define BMI160_DATA_LENGTH 12
#define ADS8688_DATA_LENGTH 16
#define GPS_DATA_LENGTH 92

#define CAN_DATA_LENGTH (BMI160_DATA_LENGTH + ADS8688_DATA_LENGTH + GPS_DATA_LENGTH) // 120
#define CAN_NUM_MESSAGES 15

// 制御フレーム (drive-controller 宛)。ID と byte 配置は kz-can can.yaml / drive-controller と一致:
//   byte0 = ETC mode / byte1 = launch(0x01=on) / byte2 = auto-shift(0x01=on)
#define CAN_ID_CONTROL 0x740

// 0x740 byte0 (ETC mode) の値。
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
    CanMaster(Bmi160 &bmi160, Ads8688 &ads8688, GPS &gps);
    esp_err_t initialize();
    esp_err_t send();
    void run();
    // センサキャッシュ保護用ミューテックス。loop()(別コア)のセンサ更新と
    // CAN タスクの getData() 読み出しを排他するため共有する。
    SemaphoreHandle_t getSensorMutex() { return sensorMutex; }

private:
    CanBus bus = CanBus();
    Bmi160 &bmi160;
    Ads8688 &ads8688;
    GPS &gps;
    uint8_t data[CAN_DATA_LENGTH];
    SemaphoreHandle_t sensorMutex = nullptr;

    // 制御スイッチ (CAN タスク内で read/送信するため別コア共有なし)。
    SelectSwitch3Pin modeSwitch = SelectSwitch3Pin(MODE_SELECT_SW_PIN_1, MODE_SELECT_SW_PIN_2, MODE_SELECT_SW_PIN_3);
    ToggleSwitch launchSwitch = ToggleSwitch(LAUNCH_SW_PIN);
    ToggleSwitch autoShiftSwitch = ToggleSwitch(AUTO_SHIFT_SW_PIN);

    void getData();
    esp_err_t sendControl();  // スイッチ状態を 0x740 制御フレームとして送信
};

void startCan(void *canMaster);

#endif