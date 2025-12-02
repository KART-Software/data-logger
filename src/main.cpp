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

CanMaster canMaster = CanMaster(bmi160, ads8688, gps, wheelSpeed);
TaskHandle_t canSendTask;

void setup()
{
    Serial.begin(115200);
    delay(3000); 

    // 1. SPI
    spi2.initialize();

    // 2. BMI160
    bmi160.initialize(); 

    // 3. ADS8688
    ads8688.initialize(); 

    // 4. GPS (繋ぐまではスキップ推奨)
    // gps.initialize(); 

    // 5. WheelSpeed 
    if (wheelSpeed.initialize()) {
        Serial.println("OK");
    } else {
        Serial.println("FAILED");
    }

    // 6. CAN
    canMaster.initialize();
    xTaskCreatePinnedToCore(startCan, "CanSendTask", 8192, (void *)&canMaster, 1, &canSendTask, 0);
}

void loop()
{
    // 各センサー更新
    // gps.tryGetGps(); 
    bmi160.getAccelGyro();
    ads8688.read();
    
    // 車輪速の計算
    //wheelSpeed.process();
    
    delay(1);
}