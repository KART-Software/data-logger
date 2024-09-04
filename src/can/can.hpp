#ifndef _CAN_HPP_
#define _CAN_HPP_

#include <driver/twai.h>
#include "config.hpp"

class CanBus
{

public:
    CanBus(uint8_t txPin = CAN_TX_PIN, uint8_t rxPin = CAN_RX_PIN);
    esp_err_t initialize();
    esp_err_t send(uint16_t id, uint8_t dlc, uint8_t *data);

private:
    uint8_t txPin, rxPin;
    twai_general_config_t g_config;
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
};

#endif