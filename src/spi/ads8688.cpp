#include "ads8688.hpp"

Ads8688::Ads8688(spi_host_device_t host, spi_device_interface_config_t deviceConfig) : SpiDevice(host, deviceConfig)
{
}

esp_err_t Ads8688::initialize()
{
    esp_err_t err = SpiDevice::initialize();
    if (err != ESP_OK)
    {
        return err;
    }
    err = reset();
    if (err != ESP_OK)
    {
        return err;
    }
    err = setReadRanges();
    if (err != ESP_OK)
    {
        return err;
    }
    err = setReadModeAutoSeq();
    return err;
}

esp_err_t Ads8688::reset()
{
    uint16_t _;
    return writeCommandRegister(RST, &_);
}

esp_err_t Ads8688::setReadRanges()
{
    esp_err_t err = 0;
    err |= writeProgramRegister(RANGE_SELECT_ADDR_0, RANGE_4);
    err |= writeProgramRegister(RANGE_SELECT_ADDR_1, RANGE_4);
    err |= writeProgramRegister(RANGE_SELECT_ADDR_2, RANGE_4);
    err |= writeProgramRegister(RANGE_SELECT_ADDR_3, RANGE_4);
    err |= writeProgramRegister(RANGE_SELECT_ADDR_4, RANGE_4);
    err |= writeProgramRegister(RANGE_SELECT_ADDR_5, RANGE_4);
    err |= writeProgramRegister(RANGE_SELECT_ADDR_6, RANGE_4);
    err |= writeProgramRegister(RANGE_SELECT_ADDR_7, RANGE_4);
    return err;
}

esp_err_t Ads8688::setReadModeAutoSeq()
{
    uint16_t _;
    return writeCommandRegister(AUTO_RST, &_);
}

esp_err_t Ads8688::read()
{
    esp_err_t err = 0;
    for (char i = 0; i < 7; i++)
    {
        err |= writeCommandRegister(NO_OP, rawValues + i);
    }
    err |= writeCommandRegister(AUTO_RST, rawValues + 7);
    return err;
}

double Ads8688::getVoltage(uint8_t ch)
{
    return rawValues[ch] * 7.8125e-05; // * 5.12 / 65536
}

uint16_t Ads8688::getRawValue(uint8_t ch)
{
    return rawValues[ch];
}

esp_err_t Ads8688::writeProgramRegister(uint8_t addr /* 7 bits */, uint8_t txdata)
{
    uint8_t data[2];
    esp_err_t err = writeReadFullDuplex(0, (addr << 1) | 0b1, &txdata, data, 16, 16);
    uint8_t res = (data[0] << 7) | (data[1] >> 1);
    err |= txdata != res;
    return err;
}

esp_err_t Ads8688::writeCommandRegister(uint8_t addr /* 8 bits */, uint16_t *rxdata)
{
    uint8_t data[3];
    esp_err_t err = writeReadFullDuplex(0, addr, NULL, data, 24, 24);
    *rxdata = (data[0] << 15) | (data[1] << 7) | (data[2] >> 1);
    return err;
}
