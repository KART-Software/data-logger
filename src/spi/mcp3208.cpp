#include "mcp3208.hpp"

Mcp3208::Mcp3208(spi_host_device_t host, spi_device_interface_config_t deviceConfig) : SpiDevice(host, deviceConfig)
{
}

esp_err_t Mcp3208::initialize()
{
    return SpiDevice::initialize();
}

esp_err_t Mcp3208::read()
{
    esp_err_t err = ESP_OK;
    esp_err_t err_;
    for (uint8_t i = 0; i < 8; i++)
    {
        err_ = SpiDevice::read(1, 0b1000 | i, rawValues + i, 12);
        if (err_ != ESP_OK)
        {
            err = ESP_FAIL;
        }
    }
    return err;
}

double Mcp3208::getVoltage(uint8_t ch)
{
    return rawValues[ch] * 0.001220703125;
}