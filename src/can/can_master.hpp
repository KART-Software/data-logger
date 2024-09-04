#ifndef _CAN_MASTER_H_
#define _CAN_MASTER_H_

#include "can.hpp"
#include "spi/bmi160.hpp"
#include "spi/ads8688.hpp"
#include "gps/gps.hpp"

#define CAN_ID_START 0x700

#define BMI160_DATA_LENGTH 12
#define ADS8688_DATA_LENGTH 16
#define GPS_DATA_LENGTH 92

#define CAN_DATA_LENGTH (BMI160_DATA_LENGTH + ADS8688_DATA_LENGTH + GPS_DATA_LENGTH) // 120
#define CAN_NUM_MESSAGES 15

class CanMaster
{
public:
    CanMaster(Bmi160 &bmi160, Ads8688 &ads8688, GPS &gps);
    esp_err_t initialize();
    esp_err_t send();
    void run();

private:
    CanBus bus = CanBus();
    Bmi160 &bmi160;
    Ads8688 &ads8688;
    GPS &gps;
    uint8_t data[CAN_DATA_LENGTH];

    void getData();
};

void startCan(void *canMaster);

#endif