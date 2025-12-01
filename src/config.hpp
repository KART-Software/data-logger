#ifndef _CONFIG_H_
#define _CONFIG_H_

///////////
/// SPI ///
///////////

#define SPI_2_MOSI_PIN 15
#define SPI_2_MISO_PIN 17
#define SPI_2_SCLK_PIN 7

#define SPI_3_MOSI_PIN 9
#define SPI_3_MISO_PIN 46
#define SPI_3_SCLK_PIN 3

#define BMI160_SPI_HOST SPI2_HOST
#define BMI160_SPI_CS_PIN 16

#define MCP3208_SPI_HOST SPI2_HOST
#define MCP3208_SPI_CS_PIN 18

#define ADS8688_SPI_HOST SPI2_HOST
#define ADS8688_SPI_CS_PIN 18

#define NEO_M8U_SPI_HOST SPI3_HOST
#define NEO_M8U_SPI_CS_PIN 10

///////////
/// CAN ///
///////////

#define CAN_TX_PIN 35
#define CAN_RX_PIN 36

///////////
/// GPS ///
///////////

#define GPS_MODULE_NEO_M8U
// #define GPS_MODULE_ZOE_M8Q

//////////////////
/// WHELLSPEED ///
//////////////////

#define WHEELSPEED_F0_PIN 13
#define WHEELSPEED_F1_PIN 12
#define WHEELSPEED_F2_PIN 21
#define WHEELSPEED_F3_PIN 14
#define WHEELSPEED_F4_PIN 47
#define WHEELSPEED_F5_PIN 48
#define WHEELDIAMETER 0.398 // need to change
#define NUM_OF_TEETH 10   // need to change

#endif