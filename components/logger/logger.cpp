#include "logger.hpp"

/*
 * Initialize logging system
 * Set global log level here
 */
void logger_init()
{
    // Default: INFO level logs
    esp_log_level_set("*", ESP_LOG_INFO);

    // You can fine-tune per module if needed
    // esp_log_level_set("WIFI", ESP_LOG_DEBUG);
    // esp_log_level_set("MQTT", ESP_LOG_DEBUG);

    ESP_LOGI("LOGGER", "Logger initialized");
}
