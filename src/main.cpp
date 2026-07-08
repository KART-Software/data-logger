#include "config.hpp"
#include "spi/spi.hpp"
#include "spi/bmi160.hpp"
#include "spi/ads8688.hpp"
#include "gps/gps.hpp"
#include <Arduino.h>
#include "can/can_master.hpp"
#include "util/toggle_switch.hpp"

SpiBus spi2 = SpiBus(SPI2_HOST, SPI_BUS_2_CONFIG);
Bmi160 bmi160 = Bmi160();
Ads8688 ads8688 = Ads8688();
GPS gps;

// 制御スイッチ (入力デバイス)。サンプリング(.read())は loop() で行い、
// CanMaster は状態を読むだけ (センサと同じ扱い)。
SelectSwitch3Pin modeSwitch = SelectSwitch3Pin(MODE_SELECT_SW_PIN_1, MODE_SELECT_SW_PIN_2, MODE_SELECT_SW_PIN_3);
ToggleSwitch launchSwitch = ToggleSwitch(LAUNCH_SW_PIN);
ToggleSwitch autoShiftSwitch = ToggleSwitch(AUTO_SHIFT_SW_PIN);

CanMaster canMaster = CanMaster(bmi160, ads8688, gps, modeSwitch, launchSwitch, autoShiftSwitch);
TaskHandle_t canSendTask;

void setup()
{
    delay(3000);
    Serial.begin(115200);
    spi2.initialize();

    uint8_t err = bmi160.initialize();
    Serial.printf("bmi160 initialize: %d\n", err);
    esp_err_t err_ = ads8688.initialize();
    Serial.printf("ads8688 initialize: %d\n", err_);

    gps.initialize();

    modeSwitch.initialize();
    launchSwitch.initialize();
    autoShiftSwitch.initialize();

    esp_err_t canErr = canMaster.initialize();
    Serial.printf("can initialize: %d\n", canErr);
    if (canErr == ESP_OK)
    {
        xTaskCreatePinnedToCore(startCan, "CanSendTask", 8192, (void *)&canMaster, 1, &canSendTask, 0);
    }
    else
    {
        Serial.println("CAN init failed; CanSendTask not started");
    }
}

void loop()
{
    // センサ・スイッチのサンプリングは CAN タスク(別コア)の状態読み出しと排他する。
    SemaphoreHandle_t m = canMaster.getSensorMutex();
    if (m != nullptr)
    {
        xSemaphoreTake(m, portMAX_DELAY);
    }
    bool gpsValid = gps.tryGetGps();
    AccelGyro ag = bmi160.getAccelGyro();
    // if (ag.bmi160OK == BMI160_OK)
    // {
    //     Serial.printf("accel gyro %6.2d %6.2d %6.2d %6.2d %6.2d %6.2d ", ag.accel.x, ag.accel.y, ag.accel.z, ag.gyro.x, ag.gyro.y, ag.gyro.z);
    // }
    esp_err_t err = ads8688.read();
    // if (err == ESP_OK)
    // {
    //     Serial.printf("vol: %f\n", ads8688.getVoltage(6));
    // }
    // 制御スイッチのサンプリング(デバウンス)。状態は CanMaster が送信時に読む。
    modeSwitch.read();
    launchSwitch.read();
    autoShiftSwitch.read();
    if (m != nullptr)
    {
        xSemaphoreGive(m);
    }

    delay(1);
}
