#include "spi.hpp"

SpiDevice::SpiDevice(spi_host_device_t host, spi_device_interface_config_t deviceConfig) : host(host), deviceConfig(deviceConfig), deviceHandle(spi_device_handle_t())
{
}

esp_err_t SpiDevice::initialize()
{
    return spi_bus_add_device(host, &deviceConfig, &deviceHandle);
}

esp_err_t SpiDevice::write(uint16_t command, uint64_t address, const void *data, size_t bits)
{
    spi_transaction_t trans = {
        .cmd = command,
        .addr = address,
        .length = bits,
        .rxlength = 0,
        .tx_buffer = data,
        .rx_buffer = NULL,
    };
    return spi_device_transmit(deviceHandle, &trans);
}
esp_err_t SpiDevice::read(uint16_t command, uint64_t address, void *data, size_t bits)
{
    spi_transaction_t trans = {
        .cmd = command,
        .addr = address,
        .length = 0,
        .rxlength = bits,
        .tx_buffer = NULL,
        .rx_buffer = data,
    };
    return spi_device_transmit(deviceHandle, &trans);
}

esp_err_t SpiDevice::writeVariableBits(uint16_t command, size_t commandLen, uint64_t address, size_t addressLen, size_t dummyLen, const void *data, size_t bits)
{
    spi_transaction_ext_t trans = {
        .base = {
            .flags = SPI_TRANS_VARIABLE_CMD | SPI_TRANS_VARIABLE_ADDR,
            .cmd = command,
            .addr = address,
            .length = bits,
            .tx_buffer = data,
            .rx_buffer = NULL,
        },
        .command_bits = commandLen,
        .address_bits = addressLen,
        .dummy_bits = dummyLen,
    };
    return spi_device_queue_trans(deviceHandle, (spi_transaction_t *)&trans, portMAX_DELAY);
}

esp_err_t SpiDevice::readVariableBits(uint16_t command, size_t commandLen, uint64_t address, size_t addressLen, size_t dummyLen, void *data, size_t bits)
{
    spi_transaction_ext_t trans = {
        .base = {
            .flags = SPI_TRANS_VARIABLE_CMD | SPI_TRANS_VARIABLE_ADDR,
            .cmd = command,
            .addr = address,
            .length = bits,
            .tx_buffer = NULL,
            .rx_buffer = data,
        },
        .command_bits = commandLen,
        .address_bits = addressLen,
        .dummy_bits = dummyLen,
    };
    return spi_device_queue_trans(deviceHandle, (spi_transaction_t *)&trans, portMAX_DELAY);
}

SpiBus::SpiBus(spi_host_device_t host, spi_bus_config_t busConfig) : host(host), busConfig(busConfig)
{
}

esp_err_t SpiBus::initialize()
{
    return spi_bus_initialize(host, &busConfig, SPI_DMA_DISABLED);
}