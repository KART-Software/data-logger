#include "bmi160.hpp"

Bmi160::Bmi160(spi_host_device_t host, spi_device_interface_config_t deviceConfig) : SpiDevice(host, deviceConfig)
{
    Bmi160::self = this;
}

int8_t Bmi160::initialize()
{
    esp_err_t err_ = SpiDevice::initialize();
    if (err_ != ESP_OK)
    {
        return err_;
    }
    uint8_t err = bmi160_init(&bmi160Dev);
    if (err != BMI160_OK)
    {
        return err;
    }
    err = bmi160_set_sens_conf(&bmi160Dev);
    if (err != BMI160_OK)
    {
        return err;
    }
    return ESP_OK;
}

AccelGyro Bmi160::getAccelGyro()
{
    AccelGyro ag;
    ag.bmi160OK = bmi160_get_sensor_data(BMI160_ACCEL_SEL | BMI160_GYRO_SEL, &ag.accel, &ag.gyro, &bmi160Dev);
    return ag;
}

int8_t Bmi160::spiWrite(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    return self->write(0, reg_addr, data, len * 8);
}

int8_t Bmi160::spiRead(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    return self->read(0, reg_addr, data, len * 8);
}