#ifndef _CONFIG_H_
#define _CONFIG_H_

///////////
/// SPI ///
///////////

#define SPI_2_MOSI_PIN 11
#define SPI_2_MISO_PIN 13
#define SPI_2_SCLK_PIN 12

#define SPI_3_MOSI_PIN 17
#define SPI_3_MISO_PIN 16
#define SPI_3_SCLK_PIN 15

#define BMI160_SPI_HOST SPI2_HOST
#define BMI160_SPI_CS_PIN 14

#define MCP3208_SPI_HOST SPI2_HOST
#define MCP3208_SPI_CS_PIN 10

#define ADS8688_SPI_HOST SPI2_HOST
#define ADS8688_SPI_CS_PIN 10

#define NEO_M8U_SPI_HOST SPI3_HOST
#define NEO_M8U_SPI_CS_PIN 18

///////////
/// CAN ///
///////////

#define CAN_TX_PIN 35
#define CAN_RX_PIN 36

#endif