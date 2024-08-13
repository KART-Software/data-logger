#ifndef _SPI_HPP_
#define _SPI_HPP_
#include <driver/spi_master.h>
#include <hal/spi_types.h>
#include "config.hpp"

class SpiDevice
{
public:
    SpiDevice(spi_host_device_t host, spi_device_interface_config_t deviceConfig);
    esp_err_t initialize();

private:
    const spi_host_device_t host;
    const spi_device_interface_config_t deviceConfig;
    spi_device_handle_t deviceHandle;

protected:
    esp_err_t write(uint16_t command, uint64_t address, const void *data, size_t bits);
    esp_err_t read(uint16_t command, uint64_t address, void *data, size_t bits);
    esp_err_t writeVariableBits(uint16_t command, size_t commandLen, uint64_t address, size_t addressLen, size_t dummyLen, const void *data, size_t bits);
    esp_err_t readVariableBits(uint16_t command, size_t commandLen, uint64_t address, size_t addressLen, size_t dummyLen, void *data, size_t bits);
};

#define SPI_BUS_2_CONFIG                                \
    {                                                   \
        {.mosi_io_num = SPI_2_MOSI_PIN},                \
        {.miso_io_num = SPI_2_MISO_PIN},                \
        .sclk_io_num = SPI_2_SCLK_PIN,                  \
        {.quadwp_io_num = -1},                          \
        {.quadhd_io_num = -1},                          \
        .data4_io_num = -1,                             \
        .data5_io_num = -1,                             \
        .data6_io_num = -1,                             \
        .data7_io_num = -1,                             \
        .max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE, \
        .flags = 0,                                     \
        .intr_flags = 0,                                \
    }

#define SPI_BUS_3_CONFIG                                \
    {                                                   \
        {.mosi_io_num = SPI_3_MOSI_PIN},                \
        {.miso_io_num = SPI_3_MISO_PIN},                \
        .sclk_io_num = SPI_3_SCLK_PIN,                  \
        {.quadwp_io_num = -1},                          \
        {.quadhd_io_num = -1},                          \
        .data4_io_num = -1,                             \
        .data5_io_num = -1,                             \
        .data6_io_num = -1,                             \
        .data7_io_num = -1,                             \
        .max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE, \
        .flags = 0,                                     \
        .intr_flags = 0,                                \
    }

class SpiBus
{
public:
    SpiBus(spi_host_device_t host, spi_bus_config_t busConfig);
    esp_err_t initialize();

private:
    const spi_host_device_t host;
    const spi_bus_config_t busConfig;
};

#endif