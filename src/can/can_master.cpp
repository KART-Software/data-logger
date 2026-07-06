#include "can_master.hpp"
#include "freertos/task.h"

// GPS 領域のバッファ長は u-blox NAV-PVT ペイロード長と一致していなければ
// GPS::getBytes の memcpy が data[] を溢れる。コンパイル時に固定する。
static_assert(GPS_DATA_LENGTH == UBX_NAV_PVT_LEN,
              "GPS_DATA_LENGTH must equal UBX_NAV_PVT_LEN");
static_assert(CAN_NUM_MESSAGES == (CAN_DATA_LENGTH + 7) / 8,
              "CAN_NUM_MESSAGES must match the number of 8-byte frames");

CanMaster::CanMaster(Bmi160 &bmi160, Ads8688 &ads8688, GPS &gps) : bmi160(bmi160), ads8688(ads8688), gps(gps)
{
}

esp_err_t CanMaster::initialize()
{
    sensorMutex = xSemaphoreCreateMutex();
    return bus.initialize();
}

void CanMaster::getData()
{
    if (sensorMutex != nullptr)
    {
        xSemaphoreTake(sensorMutex, portMAX_DELAY);
    }
    bmi160.getBytes(data, 0);
    ads8688.getBytes(data, BMI160_DATA_LENGTH);
    // GPS が有効な UBX を持たないと getBytes は false を返し、GPS 領域は前回値のまま。
    // 受信側は iTOW が進まないことで stale を検知できる (明示フラグ化は要ワイヤ仕様調整)。
    gps.getBytes(data, BMI160_DATA_LENGTH + ADS8688_DATA_LENGTH);
    if (sensorMutex != nullptr)
    {
        xSemaphoreGive(sensorMutex);
    }
}

esp_err_t CanMaster::send()
{
    // 1フレームずつブロッキング送信 (キュー満杯なら空くまで待つ)。delay 不要で
    // バスの限界速度まで詰めて送れ、送信本数が増えても破綻しない。
    esp_err_t firstErr = ESP_OK;
    for (int i = 0; 8 * i < CAN_DATA_LENGTH; i++)
    {
        esp_err_t err = bus.send(i + CAN_ID_START, min(8, CAN_DATA_LENGTH - 8 * i), &data[8 * i]);
        if (err != ESP_OK && firstErr == ESP_OK)
        {
            firstErr = err;
        }
    }
    return firstErr;
}

void CanMaster::run()
{
    TickType_t lastWake = xTaskGetTickCount();
    while (true)
    {
        // 正確な ~30Hz 周期で起床し、待機中は CPU を明け渡す (ビジーウェイト回避)。
        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(33));
        bus.recover();  // bus-off に陥っていたら復帰を試みる
        getData();
        send();
    }
}

void startCan(void *canMaster)
{
    CanMaster *canMaster_ = (CanMaster *)canMaster;
    canMaster_->run();
}