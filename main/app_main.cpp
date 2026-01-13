#include "wifi_manager.hpp"
#include "wifi_web_server.hpp"
#include "relay_controller.hpp"
#include "mqtt_manager.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "APP_MAIN";

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "🚀 System Booting...");

    // 1️⃣ Wi-Fi init (AP + STA)
    WifiManager::init();

    // 2️⃣ Web UI always available
    WifiWebServer::start();

    // 3️⃣ Relay GPIO init
    RelayController::init();

    // 4️⃣ Wait until Wi-Fi is connected
    while (!WifiManager::is_connected()) {
        ESP_LOGI(TAG, "⏳ Waiting for Wi-Fi connection...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    ESP_LOGI(TAG, "✅ Wi-Fi connected");

    // Optional safety delay
    vTaskDelay(pdMS_TO_TICKS(500));

    // 5️⃣ Start MQTT
    MQTTManager::start();

    ESP_LOGI(TAG, "📡 MQTT started");
}
