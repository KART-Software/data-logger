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
#define CAN_TX_QUEUE_LEN 32    // 1サイクルの最大送信本数以上に (現状15本 + 将来余裕)
#define CAN_TX_TIMEOUT_MS 10   // 送信キュー空き待ちの上限[ms]。0 だと満杯時にフレーム脱落

//////////////////////
/// 制御スイッチ入力 ///
//////////////////////

// drive-controller への制御フレーム(0x740)の入力源。
// TODO: 暫定ピン。実配線で確定する (SPI2:11-14 / SPI3:15-18 / CS:10,14,18 / CAN:35,36 は使用済み)。
#define MODE_SELECT_SW_PIN_1 4  // SelectSwitch3Pin: First
#define MODE_SELECT_SW_PIN_2 5  // SelectSwitch3Pin: Second
#define MODE_SELECT_SW_PIN_3 6  // SelectSwitch3Pin: Third
#define LAUNCH_SW_PIN 7         // ToggleSwitch: launch
#define AUTO_SHIFT_SW_PIN 8     // ToggleSwitch: auto-shifter

///////////
/// GPS ///
///////////

#define GPS_MODULE_NEO_M8U
// #define GPS_MODULE_ZOE_M8Q

#endif