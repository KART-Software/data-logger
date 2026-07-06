#include "can.hpp"
#include <cstring>
#include "freertos/FreeRTOS.h"

CanBus::CanBus(uint8_t txPin, uint8_t rxPin) : txPin(txPin), rxPin(rxPin)
{
    g_config = TWAI_GENERAL_CONFIG_DEFAULT(gpio_num_t(txPin), gpio_num_t(rxPin), TWAI_MODE_NORMAL);
    g_config.tx_queue_len = CAN_TX_QUEUE_LEN;
}

esp_err_t CanBus::initialize()
{
    esp_err_t err = twai_driver_install(&g_config, &t_config, &f_config);
    if (err != ESP_OK)
    {
        return err;
    }

    err = twai_start();
    if (err != ESP_OK)
    {
        return err;
    }
    return ESP_OK;
}

esp_err_t CanBus::send(uint16_t id, uint8_t dlc, const uint8_t *data)
{
    twai_message_t message = {};  // 全フラグ0クリア: ss=0(自動再送) / extd=0(標準11bit) / reserved=0
    message.identifier = id;
    message.data_length_code = dlc;
    memcpy(message.data, data, dlc);
    // ブロッキング送信: キューが満杯なら空くまで待つ (脱落させない)。
    // タイムアウトは有限値: バス断/BUS_OFF 時に無限ブロックしないため。
    return twai_transmit(&message, pdMS_TO_TICKS(CAN_TX_TIMEOUT_MS));
}

void CanBus::recover()
{
    twai_status_info_t status;
    if (twai_get_status_info(&status) != ESP_OK)
    {
        return;
    }
    if (status.state == TWAI_STATE_BUS_OFF)
    {
        twai_initiate_recovery();  // 復帰シーケンス開始 (完了後 STOPPED になる)
    }
    else if (status.state == TWAI_STATE_STOPPED)
    {
        twai_start();  // 復帰完了後に送信を再開
    }
}
