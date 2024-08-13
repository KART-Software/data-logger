#include "ads8688.hpp"

Ads8688::Ads8688(spi_host_device_t host = ADS8688_SPI_HOST, spi_device_interface_config_t deviceConfig = BMI160_SPI_DEVICE_CONFIG) : SpiDevice(host, deviceConfig)
{
}

esp_err_t Ads8688::initialize()
{
    setReadRanges();
    setReadModeAutoSeq();
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
    for (char i = 0; i < 7; i++)
    {
        writeCommandRegister(NO_OP, rawValues + i);
    }
    writeCommandRegister(AUTO_RST, rawValues + 7);
}

double Ads8688::getVoltage(uint8_t ch)
{
    return rawValues[ch] * 7.8125e-05; // * 5.12 / 65536
}

esp_err_t Ads8688::writeProgramRegister(uint8_t addr /* 7 bits */, uint8_t data)
{
    uint16_t data_ = ((uint16_t)data) << 8;
    return writeVariableBits(addr, 7, 1, 1, 0, &data_, 16);
}

esp_err_t Ads8688::writeCommandRegister(uint16_t addr /* 16 bits */, uint16_t *data)
{
    return SpiDevice::read(0, addr, data, 16);
}