#include "config.hpp"
#include "spi/spi.hpp"
#include "spi/bmi160.hpp"
#include "spi/ads8688.hpp"
#include "gps/gps.hpp"
#include <Arduino.h>
#include "can/can_master.hpp"
#include <driver/pcnt.h>
#include "wheel_speed/wheel_speed.hpp"

SpiBus spi2 = SpiBus(SPI2_HOST, SPI_BUS_2_CONFIG);
Bmi160 bmi160 = Bmi160();
Ads8688 ads8688 = Ads8688();
GPS gps;
WheelSpeed wheelSpeed;

CanMaster canMaster = CanMaster(bmi160, ads8688, wheelSpeed, gps);
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

    // 車輪速センサの初期化
    if (wheelSpeed.initialize()) {
        Serial.println("WheelSpeed Initialized");
    } else {
        Serial.println("WheelSpeed Init Failed");
    }

    canMaster.initialize();
    xTaskCreatePinnedToCore(startCan, "CanSendTask", 8192, (void *)&canMaster, 1, &canSendTask, 0);
}

void loop()
{
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
    //
    wheelSpeed.getWheelSpeed();

    // 動作確認用のシリアル表示 (100msごとに表示)
    static unsigned long lastPrintTime = 0;
    if (millis() - lastPrintTime > 100)
    {
        lastPrintTime = millis();
        
        // 4輪の速度を表示
        Serial.printf("FL: %.1f, FR: %.1f, RL: %.1f, RR: %.1f [km/h]\n", 
            wheelSpeed.getSpeed(0), 
            wheelSpeed.getSpeed(1), 
            wheelSpeed.getSpeed(2), 
            wheelSpeed.getSpeed(3));

        // デバッグ用: もし速度が出ないときはRawカウントを確認してみてください
        // Serial.printf("Cnt: %d %d %d %d\n", 
        //     wheelSpeed.getCount(0), wheelSpeed.getCount(1), 
        //     wheelSpeed.getCount(2), wheelSpeed.getCount(3));
    }
    delay(1);
}
