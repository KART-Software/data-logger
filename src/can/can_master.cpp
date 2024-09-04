#include "can_master.hpp"

CanMaster::CanMaster(Bmi160 &bmi160, Ads8688 &ads8688, GPS &gps) : bmi160(bmi160), ads8688(ads8688), gps(gps)
{
}

esp_err_t CanMaster::initialize()
{
    return bus.initialize();
}

void CanMaster::getData()
{
    bmi160.getBytes(data, 0);
    ads8688.getBytes(data, BMI160_DATA_LENGTH);
    gps.getBytes(data, BMI160_DATA_LENGTH + ADS8688_DATA_LENGTH);
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