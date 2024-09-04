#include "bmi160.hpp"

Bmi160 *Bmi160::self;

Bmi160::Bmi160(spi_host_device_t host, spi_device_interface_config_t deviceConfig) : SpiDevice(host, deviceConfig)
{
    self = this;
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
    setSensorConfig(&bmi160Dev);
    err = bmi160_set_sens_conf(&bmi160Dev);
    if (err != BMI160_OK)
    {
        return err;
    }
    return ESP_OK;
}

void Bmi160::setSensorConfig(bmi160_dev *dev)
{
    dev->accel_cfg = ACCEL_CONFIG;
    dev->gyro_cfg = GYRO_CONFIG;
}

AccelGyro Bmi160::getAccelGyro()
{
    AccelGyro ag;
    ag.bmi160OK = bmi160_get_sensor_data(BMI160_ACCEL_SEL | BMI160_GYRO_SEL, &ag.accel, &ag.gyro, &bmi160Dev);
    accelGyro = ag;
    return ag;
}

int8_t Bmi160::spiWrite(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    esp_err_t err = self->write(0, reg_addr, data, len * 8);
    assert(err == ESP_OK);
    return 0;
}

int8_t Bmi160::spiRead(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    esp_err_t err = self->read(0, reg_addr, data, len * 8);
    assert(err == ESP_OK);
    return 0;
}

void Bmi160::getBytes(uint8_t *bytes, uint startByte)
{
    bytes[startByte] = (accelGyro.accel.x >> 8) & 0xFF;
    bytes[startByte + 1] = accelGyro.accel.x & 0xFF;
    bytes[startByte + 2] = (accelGyro.accel.y >> 8) & 0xFF;
    bytes[startByte + 3] = accelGyro.accel.y & 0xFF;
    bytes[startByte + 4] = (accelGyro.accel.z >> 8) & 0xFF;
    bytes[startByte + 5] = accelGyro.accel.z & 0xFF;

    bytes[startByte + 6] = (accelGyro.gyro.x >> 8) & 0xFF;
    bytes[startByte + 7] = accelGyro.gyro.x & 0xFF;
    bytes[startByte + 8] = (accelGyro.gyro.x >> 8) & 0xFF;
    bytes[startByte + 9] = accelGyro.gyro.x & 0xFF;
    bytes[startByte + 10] = (accelGyro.gyro.x >> 8) & 0xFF;
    bytes[startByte + 11] = accelGyro.gyro.x & 0xFF;
}