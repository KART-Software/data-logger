#ifndef _MCP3208_H_
#define _MCP3208_H_

#include "spi.hpp"
#include "config.hpp"

#define MCP3208_SPI_DEVICE_CONFIG           \
    {                                       \
        .command_bits = 1,                  \
        .address_bits = 4,                  \
        .dummy_bits = 2,                    \
        .mode = 0,                          \
        .duty_cycle_pos = 128,              \
        .cs_ena_pretrans = 0,               \
        .cs_ena_posttrans = 0,              \
        .clock_speed_hz = 2000000,          \
        .input_delay_ns = 0,                \
        .spics_io_num = MCP3208_SPI_CS_PIN, \
        .flags = SPI_DEVICE_HALFDUPLEX,     \
        .queue_size = 1,                    \
    }

class Mcp3208 : SpiDevice
{
public:
    Mcp3208(spi_host_device_t host = MCP3208_SPI_HOST, spi_device_interface_config_t deviceConfig = MCP3208_SPI_DEVICE_CONFIG);
    esp_err_t initialize();
    esp_err_t read();
    double getVoltage(uint8_t ch);

private:
    uint16_t rawValues[8];
};

#endif