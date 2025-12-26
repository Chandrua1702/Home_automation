#include "relay_controller.hpp"
#include "wifi_manager.hpp"
#include "web_server.hpp"
#include "mqtt_manager.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "APP_MAIN";

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "Starting Home Automation");

    RelayController::init();
    WifiManager::init();
    WebServer::init();
    MQTTManager::start();

    while(true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
