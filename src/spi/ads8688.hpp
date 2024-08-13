#ifndef _ADC_H_
#define _ADC_H_

#include "config.hpp"
#include "spi.hpp"

#define ADS8688_SPI_DEVICE_CONFIG           \
    {                                       \
        .command_bits = 0,                  \
        .address_bits = 16,                 \
        .dummy_bits = 0,                    \
        .mode = 1,                          \
        .duty_cycle_pos = 128,              \
        .cs_ena_pretrans = 1,               \
        .cs_ena_posttrans = 0,              \
        .clock_speed_hz = 5000000,          \
        .input_delay_ns = 0,                \
        .spics_io_num = ADS8688_SPI_CS_PIN, \
        .flags = SPI_DEVICE_HALFDUPLEX,     \
        .queue_size = 1,                    \
    }
// ADS8688の仕様で先にaddressで後にcommandなので，commandとaddressを入れ替えて使う．
// ADS8688ではaddressが7bit，commandが1bitである．

#define ADC_NUM_CH 8

#define NUM_COMMAND_REGISTER_BITS 32
#define NUM_PROGRAM_REGISTER_BITS 24

#define NUM_COMMAND_REGISTER_READ_BITS 16
#define NUM_PROGRAM_REGISTER_WRITE_BITS 8

#define AUTO_SEQ_EN_ADDR 0x01

/// Registers
#define NO_OP 0x0000
#define AUTO_RST 0xA000

/// Range Select

#define RANGE_SELECT_ADDR_0 0x05
#define RANGE_SELECT_ADDR_1 0x06
#define RANGE_SELECT_ADDR_2 0x07
#define RANGE_SELECT_ADDR_3 0x08
#define RANGE_SELECT_ADDR_4 0x09
#define RANGE_SELECT_ADDR_5 0x0A
#define RANGE_SELECT_ADDR_6 0x0B
#define RANGE_SELECT_ADDR_7 0x0C

#define RANGE_0 0b0000 // ±2.5 x VREF
#define RANGE_1 0b0001 // ±1.25 x VREF
#define RANGE_2 0b0010 // ±0.625 x VREF
#define RANGE_3 0b0101 // 0 ~ 2.5 x VREF
#define RANGE_4 0b0110 // 0 ~ 1.25 x VREF
                       // VREF = 4.096V

class Ads8688 : SpiDevice
{
public:
    Ads8688(spi_host_device_t host = ADS8688_SPI_HOST, spi_device_interface_config_t deviceConfig = ADS8688_SPI_DEVICE_CONFIG);
    esp_err_t initialize();
    esp_err_t read();
    double getVoltage(uint8_t ch);

private:
    uint16_t rawValues[8];
    const uint8_t numCh = ADC_NUM_CH;
    esp_err_t writeProgramRegister(uint8_t addr, uint8_t data);
    esp_err_t writeCommandRegister(uint16_t addr, uint16_t *data);
    esp_err_t setReadModeAutoSeq();
    esp_err_t setReadRanges();
};
#endif