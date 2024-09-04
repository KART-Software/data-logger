#include "can.hpp"

CanBus::CanBus(uint8_t txPin, uint8_t rxPin) : txPin(txPin), rxPin(rxPin)
{
    g_config = TWAI_GENERAL_CONFIG_DEFAULT(gpio_num_t(txPin), gpio_num_t(rxPin), TWAI_MODE_NORMAL);
    g_config.tx_queue_len = 15;
}

esp_err_t CanBus::initialize()
{
    esp_err_t err = twai_driver_install(&g_config, &t_config, &f_config);
    if (err != ESP_OK)
    {
        return err;
    }

    err = twai_start();
    if (err != ESP_OK)
    {
        return err;
    }
    return ESP_OK;
}

esp_err_t CanBus::send(uint16_t id, uint8_t dlc, uint8_t *data)
{
    twai_message_t message;
    message.extd = 0;
    message.rtr = 0;
    message.ss = 1;
    message.self = 0;
    message.dlc_non_comp = 0;
    message.reserved = 27;
    message.identifier = id;
    message.data_length_code = dlc;
    for (int i = 0; i < message.data_length_code; i++)
    {
        message.data[i] = data[i];
    }
    return twai_transmit(&message, 0);
}
