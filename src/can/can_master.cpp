#include "can_master.hpp"

CanMaster::CanMaster(Bmi160 &bmi160, Ads8688 &ads8688, WheelSpeed &wheelSpeed, GPS &gps) : bmi160(bmi160), ads8688(ads8688), wheelSpeed(wheelSpeed), gps(gps)
{
}

esp_err_t CanMaster::initialize()
{
    return bus.initialize();
}

void CanMaster::getData()
{
    // 1. BMI160 (0 ~ 11 byte)
    bmi160.getBytes(data, 0);

    // 2. ADS8688 (12 ~ 27 byte)
    ads8688.getBytes(data, BMI160_DATA_LENGTH);

    // 3. WheelSpeed (28 ~ 43 byte)
    int ws_offset = BMI160_DATA_LENGTH + ADS8688_DATA_LENGTH;
    wheelSpeed.getBytes(data, ws_offset);

    // 4. GPS (44 ~ 135 byte)
    gps.getBytes(data, ws_offset + WHEEL_SPEED_DATA_LENGTH);
}

esp_err_t CanMaster::send()
{
    esp_err_t err = ESP_OK;
    for (int i = 0; 8 * i < CAN_DATA_LENGTH; i++)
    {
        err |= bus.send(i + CAN_ID_START, min(8, CAN_DATA_LENGTH - 8 * i), &data[8 * i]);
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