#ifndef _BMI_160_HPP_
#define _BMI_160_HPP_

#include <Arduino.h>
#include <bmi160.h>
#include "spi.hpp"
#include "config.hpp"

#define BMI160_SPI_DEVICE_CONFIG              \
    {                                         \
        .command_bits = 0,                    \
        .address_bits = 8,                    \
        .dummy_bits = 0,                      \
        .mode = 3,                            \
        .duty_cycle_pos = 128,                \
        .cs_ena_pretrans = 1,                 \
        .cs_ena_posttrans = 0,                \
        .clock_speed_hz = SPI_MASTER_FREQ_8M, \
        .input_delay_ns = 0,                  \
        .spics_io_num = BMI160_SPI_CS_PIN,    \
        .flags = SPI_DEVICE_HALFDUPLEX,       \
        .queue_size = 1,                      \
    }

struct AccelGyro
{
    uint8_t bmi160OK;
    bmi160_sensor_data accel;
    bmi160_sensor_data gyro;
};

class Bmi160 : SpiDevice
{
public:
    Bmi160(spi_host_device_t host = BMI160_SPI_HOST, spi_device_interface_config_t deviceConfig = BMI160_SPI_DEVICE_CONFIG);
    int8_t initialize();
    AccelGyro getAccelGyro();

private:
    bmi160_dev bmi160Dev = {
        .intf = BMI160_SPI_INTF,
        .accel_cfg = {
            .power = BMI160_AUX_NORMAL_MODE,
            .odr = BMI160_ACCEL_ODR_100HZ,
            .range = BMI160_ACCEL_RANGE_4G,
            .bw = BMI160_ACCEL_BW_NORMAL_AVG4,
        },
        .gyro_cfg = {
            .power = BMI160_GYRO_NORMAL_MODE,
            .odr = BMI160_GYRO_ODR_100HZ,
            .range = BMI160_GYRO_RANGE_500_DPS,
            .bw = BMI160_GYRO_BW_NORMAL_MODE,
        },
        .read = spiWrite,
        .write = spiRead,
        .delay_ms = delay};

    // bmi160のライブラリを使うために仕方なくstaticにする．
    // こうしないと
    //    .read = spiWrite,
    //    .write = spiRead,
    // のように書けない．
    static Bmi160 *self;
    static int8_t spiWrite(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);
    static int8_t spiRead(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);
};

#endif