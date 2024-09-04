#include "gps.hpp"

bool GPS::initialize(uint8_t mosi, uint8_t miso, uint8_t sclk, uint8_t cs)
{
    spi.begin(sclk, miso, mosi, cs);
    // gnss.enableDebugging();
    gnss.setFileBufferSize(GNSS_INTERNAL_FILE_BUFFER_SIZE);
    bool ok = gnss.begin(spi, cs, SPI_FREQ);
    if (!ok)
    {
        return false;
    }
#ifdef GPS_MODULE_ZOE_M8Q
    ok = gnss.factoryDefault();
    if (!ok)
    {
        return false;
    }
    delay(10);
#endif

    ok = gnss.setPortOutput(COM_PORT_SPI, COM_TYPE_UBX);
    if (!ok)
    {
        return false;
    }
    ok = gnss.setNavigationFrequency(GPS_NAVIGATION_FREQUENCY);
    if (!ok)
    {
        Serial.printf("GNSS setNavigationFrequency FAIL !!!!\nNavigation Frequency: %d\n", gnss.getNavigationFrequency());
    }
    ok = gnss.setAutoPVT(true);
    gnss.logNAVPVT(true);
    return ok;
}

bool GPS::reinitialize()
{
    return gnss.setNavigationFrequency(GPS_NAVIGATION_FREQUENCY);
}

bool GPS::tryGetGps()
{
    if (gnss.getPVT() && !gnss.getInvalidLlh())
    {
        unsigned long ms = millis();
        if (ms % 1000 < lastValidTime % 1000)
        {
            count = 0;
        }
        else
        {
            count++;
        }
        lastValidTime = ms;
        pvtData = gnss.packetUBXNAVPVT->data;
        gnss.packetUBXNAVPVT->moduleQueried.moduleQueried1.all = false;
        gnss.packetUBXNAVPVT->moduleQueried.moduleQueried1.bits.all = false;
        gnss.packetUBXNAVPVT->moduleQueried.moduleQueried2.all = false;
        return true;
    }
    return false;
}

void GPS::printPvtData()
{
    Serial.printf("ms: %d Count: %d FixType: %d Lat: %d Lon: %d (degrees * 10^-7) Alt: %d (mm) SIV: %d PDOP: %d (10^-2) VelN: %d VelE: %d VelD: %d (mm/s) VAcc: %d HAcc: %d (mm) SpeedAcc: %d (mm/s) HeadAcc: %d (degrees * 10^-5)\n",
                  millis(),
                  count,
                  pvtData.fixType,
                  pvtData.lat,
                  pvtData.lon,
                  pvtData.height,
                  pvtData.numSV,
                  pvtData.pDOP,
                  pvtData.velN,
                  pvtData.velE,
                  pvtData.velD,
                  pvtData.vAcc,
                  pvtData.hAcc,
                  pvtData.sAcc,
                  pvtData.headAcc);

    if (gnss.getHeadVehValid() == true)
    {
        Serial.printf("HeadVeh: %d (degrees * 10^-5) MagDec: %d (degrees * 10^-2) MagAcc: %d (degrees * 10^-2)\n", pvtData.headVeh, pvtData.magDec, pvtData.magAcc);
    }
}

bool GPS::getBytes(uint8_t *bytes, uint startByte)
{
    uint8_t d[GNSS_PACKET_BYTES_SIZE];
    gnss.extractFileBufferData(d, GNSS_PACKET_BYTES_SIZE);
    if (d[0] != UBX_SYNCH_1 || d[1] != UBX_SYNCH_2)
    {
        return false;
    }
    // file://./../../lib/SparkFun_u-blox_GNSS_Arduino_Library/src/SparkFun_u-blox_GNSS_Arduino_Library.cpp の 7069行目ぐらいを参照
    // 緯度経度や速度に変換する前の生のバイト列がバッファーに保存されているので，それを取得する．
    memcpy(bytes + startByte, d + 6, UBX_NAV_PVT_LEN);
    return true;
}

void GPS::startGetGps()
{

    unsigned long lastReinit = millis();
    while (true)
    {
        tryGetGps();
        if (millis() - lastReinit > 120000)
        {
            reinitialize();
        }
    }
}
