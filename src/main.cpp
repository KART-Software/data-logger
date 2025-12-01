#include "config.hpp"
#include "spi/spi.hpp"
#include "spi/bmi160.hpp"
#include "spi/ads8688.hpp"
#include "gps/gps.hpp"
#include "can/can_master.hpp"
#include <Arduino.h>
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
    Serial.begin(115200);
    delay(3000); 

    Serial.println("\n==============================");
    Serial.println(">>> WHEEL SPEED TEST START <<<");
    Serial.println("==============================");

    // 1. SPI
    Serial.print("1. SPI2 Bus... ");
    spi2.initialize();
    Serial.println("OK");

    // 2. BMI160
    Serial.print("2. BMI160... "); 
    bmi160.initialize(); 
    Serial.println("OK");

    // 3. ADS8688
    Serial.print("3. ADS8688... "); 
    ads8688.initialize(); 
    Serial.println("OK");

    // 4. GPS (繋ぐまではスキップ推奨)
    Serial.println("4. GPS... (SKIPPED)"); 
    // gps.initialize(); 

    // 5. WheelSpeed
    Serial.print("5. WheelSpeed... "); 
    if (wheelSpeed.initialize()) {
        Serial.println("OK");
    } else {
        Serial.println("FAILED");
    }

    // 6. CAN
    Serial.print("6. CAN Master... ");
    canMaster.initialize();
    xTaskCreatePinnedToCore(startCan, "CanSendTask", 8192, (void *)&canMaster, 1, &canSendTask, 0);
    Serial.println("OK");

    Serial.println(">>> SETUP DONE. ROTATE THE WHEELS! <<<");
}

void loop()
{
    // 各センサー更新
    // gps.tryGetGps(); 
    bmi160.getAccelGyro();
    ads8688.read();
    
    // ★重要: 車輪速の計算
    //wheelSpeed.getWheelSpeed();

    // 0.2秒ごとにシリアル表示
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 500) {
        lastPrint = millis();

        wheelSpeed.getWheelSpeed();

        // 速度表示
        Serial.printf("FL: %5.1f  FR: %5.1f  RL: %5.1f  RR: %5.1f [km/h]\n", 
            wheelSpeed.getSpeed(0), 
            wheelSpeed.getSpeed(1), 
            wheelSpeed.getSpeed(2), 
            wheelSpeed.getSpeed(3));
            
        // デバッグ用: もし速度が出ない時はカウント値(Raw)も見てみる
        // Serial.printf("Cnt: %d %d %d %d\n", 
        //    wheelSpeed.getCount(0), wheelSpeed.getCount(1), wheelSpeed.getCount(2), wheelSpeed.getCount(3));
    }
    
    delay(1);
}