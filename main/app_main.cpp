#include "relay_controller.hpp"
#include "wifi_manager.hpp"
#include "web_server.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"  // <- add this
#include "freertos/task.h"      // <- add this

static const char* TAG = "APP_MAIN";

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "Starting Home Automation");

    RelayController::init(); // initialize relays
    WifiManager::init();     // start Wi-Fi AP
    WebServer::init();       // start web server

    while(true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS); // keep main task alive
    }
}
