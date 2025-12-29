#include "wifi_manager.hpp"
#include "wifi_web_server.hpp"
#include "mqtt_manager.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "APP_MAIN";

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "Starting Home Automation");
    WifiManager::init();
    WifiManager::auto_connect();   // auto-connect if saved
    WifiWebServer::start();       
}
