#ifndef _GPS_H_
#define _GPS_H_

#include <SPI.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include "config.hpp"

#define SPI_FREQ 4000000

#ifdef GPS_MODULE_NEO_M8U
#define GPS_NAVIGATION_FREQUENCY 20
#endif
#ifdef GPS_MODULE_ZOE_M8Q
#define GPS_NAVIGATION_FREQUENCY 18
#endif

#define GNSS_PACKET_BYTES_SIZE (UBX_NAV_PVT_LEN + 8)
#define GNSS_INTERNAL_FILE_BUFFER_SIZE (UBX_NAV_PVT_LEN + 8 + 1)

class GPS
{
public:
    bool initialize(uint8_t mosi = SPI_3_MOSI_PIN, uint8_t miso = SPI_3_MISO_PIN, uint8_t sclk = SPI_3_SCLK_PIN, uint8_t cs = NEO_M8U_SPI_CS_PIN);
    bool reinitialize();
    bool tryGetGps();
    void printPvtData();
    void startGetGps();

    // bytes[startByte] から順にGPSデータ(92バイト)を書き込む
    bool getBytes(uint8_t *bytes, uint startByte);

private:
    SPIClass spi;
    SFE_UBLOX_GNSS gnss;
    UBX_NAV_PVT_data_t pvtData;

    // 一秒間のカウント
    int count = 0;
    unsigned long lastValidTime;
};

#endif