#include "can_master.hpp"

CanMaster::CanMaster(Bmi160 &bmi160, Ads8688 &ads8688, GPS &gps, WheelSpeed &wheelSpeed) : bmi160(bmi160), ads8688(ads8688), gps(gps), wheelSpeed(wheelSpeed)
{
}

esp_err_t CanMaster::initialize()
{
    return bus.initialize();
}

void CanMaster::getData()
{
    wheelSpeed.process();
    // 1. BMI160 (0 ~ 11 byte)
    bmi160.getBytes(data, 0);

    // 2. ADS8688 (12 ~ 27 byte)
    ads8688.getBytes(data, BMI160_DATA_LENGTH);

    // 3. GPS (44 ~ 119 byte)
    //gps.getBytes(data, BMI160_DATA_LENGTH + ADS8688_DATA_LENGTH);

    //GPS (ゼロ埋め処理)
    memset(&data[BMI160_DATA_LENGTH + ADS8688_DATA_LENGTH], 0, GPS_DATA_LENGTH);
    
    // 4. WheelSpeed (120 ~ 135 byte)
    wheelSpeed.getBytes(data, BMI160_DATA_LENGTH + ADS8688_DATA_LENGTH + GPS_DATA_LENGTH);
}

esp_err_t CanMaster::send()
{
    esp_err_t err = ESP_OK;
    for (int i = 0; 8 * i < CAN_DATA_LENGTH; i++)
    {
        int currentId = i + CAN_ID_START;
        
        // 送信実行
        esp_err_t result = bus.send(currentId, min(8, CAN_DATA_LENGTH - 8 * i), &data[8 * i]);
        
        // ★失敗したときだけログを出す（これでログの文字化けが直ります）
        if (result != ESP_OK) {
            Serial.print("FAIL ID: ");
            Serial.print(currentId, HEX);
            Serial.print(" ErrCode: ");
            Serial.println(result, HEX); // 105(Timeout) や 107(BusOff) などが出るはず
        }

        err |= result;
        delay(1);
    }
    return err;
}

void CanMaster::run()
{
    unsigned long lastSendTime = millis();
    while (true)
    {
        unsigned long ms = millis();
        if (ms - lastSendTime > 33)
        {
            lastSendTime = ms;
            getData();
            send();
        }
    }
}

void startCan(void *canMaster)
{
    CanMaster *canMaster_ = (CanMaster *)canMaster;
    canMaster_->run();
}