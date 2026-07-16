#include "can_master.hpp"
#include <kart_can.h>
#include "freertos/task.h"

// GPS 領域のバッファ長は u-blox NAV-PVT ペイロード長と一致していなければ
// GPS::getBytes の memcpy が data[] を溢れる。コンパイル時に固定する。
static_assert(GPS_DATA_LENGTH == UBX_NAV_PVT_LEN,
              "GPS_DATA_LENGTH must equal UBX_NAV_PVT_LEN");
static_assert(CAN_NUM_MESSAGES == (CAN_DATA_LENGTH + 7) / 8,
              "CAN_NUM_MESSAGES must match the number of DL frames (0x700-0x70E)");

namespace {
// モード選択スイッチのポジション → KartControl.etc_mode。
uint8_t modeToByte(SelectSwitch3Pin::Status s)
{
    switch (s)
    {
        case SelectSwitch3Pin::Status::First:  return CTRL_MODE_NORMAL;
        case SelectSwitch3Pin::Status::Second: return CTRL_MODE_RESTRICTED;
        case SelectSwitch3Pin::Status::Third:  return CTRL_MODE_MOTOR_OFF;
        case SelectSwitch3Pin::Status::Zero:
        default:                               return CTRL_MODE_CALIB;
    }
}
}  // namespace

CanMaster::CanMaster(Bmi160 &bmi160, Ads8688 &ads8688, GPS &gps,
                     SelectSwitch3Pin &modeSwitch, ToggleSwitch &launchSwitch, ToggleSwitch &autoShiftSwitch)
    : bmi160(bmi160), ads8688(ads8688), gps(gps),
      modeSwitch(modeSwitch), launchSwitch(launchSwitch), autoShiftSwitch(autoShiftSwitch)
{
}

esp_err_t CanMaster::initialize()
{
    sensorMutex = xSemaphoreCreateMutex();
    return bus.initialize();
}

void CanMaster::getData()
{
    // 各センサはすでにワイヤ形式の生バイトを書き込むので、中身を分解せず 120B
    // バッファ data[] に集約する (BMI160=BE int16 / ADS8688=BE uint16 / GPS=raw UBX-NAV-PVT)。
    if (sensorMutex != nullptr)
    {
        xSemaphoreTake(sensorMutex, portMAX_DELAY);
    }
    bmi160.getBytes(data, 0);
    ads8688.getBytes(data, BMI160_DATA_LENGTH);
    // GPS が有効な UBX を持たないと getBytes は false を返し GPS 領域は前回値のまま
    // (受信側は iTOW が進まないことで stale を検知可能)。
    gps.getBytes(data, BMI160_DATA_LENGTH + ADS8688_DATA_LENGTH);
    if (sensorMutex != nullptr)
    {
        xSemaphoreGive(sensorMutex);
    }
}

esp_err_t CanMaster::send()
{
    // 120B バッファを 8B×15 に分割送信。ID は生成の KART_CAN_DL_700_FRAME_ID
    // (0x700) から連番 (0x700-0x70E)。GPS 等の不透明ブロブは field 分解→再 pack せず
    // raw のまま流す (デコードは受信側 = kart-can DBC の責務)。
    esp_err_t firstErr = ESP_OK;
    for (int i = 0; 8 * i < CAN_DATA_LENGTH; i++)
    {
        esp_err_t err = bus.send(KART_CAN_DL_700_FRAME_ID + i, min(8, CAN_DATA_LENGTH - 8 * i), &data[8 * i]);
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
#if defined(CONTROL_OUTPUT_VIA_CAN)
        sendControl();  // スイッチ状態(loop()でサンプリング済み)から制御フレームを送信
#endif
    }
}

esp_err_t CanMaster::sendControl()
{
    // スイッチ状態のスナップショットを取得 (loop()側のサンプリングと排他)。
    // 制御フレームは論理値 (mode/launch/auto-shift) から構築するので pack が有効。
    struct kart_can_control_t c = {};
    if (sensorMutex != nullptr)
    {
        xSemaphoreTake(sensorMutex, portMAX_DELAY);
    }
    c.etc_mode = modeToByte(modeSwitch.getStatus());
    c.launch_active = launchSwitch.isOn() ? 1 : 0;
    c.auto_shift = autoShiftSwitch.isOn() ? 1 : 0;
    if (sensorMutex != nullptr)
    {
        xSemaphoreGive(sensorMutex);
    }
    uint8_t buf[3];
    kart_can_control_pack(buf, &c, sizeof(buf));
    return bus.send(KART_CAN_CONTROL_FRAME_ID, sizeof(buf), buf);
}

void startCan(void *canMaster)
{
    CanMaster *canMaster_ = (CanMaster *)canMaster;
    canMaster_->run();
}
