#include "mqtt_manager.hpp"
#include "../common/app_config.hpp"
#include "../relay_controller/relay_controller.hpp"
#include "esp_log.h"
#include "mqtt_client.h"

static const char* TAG = "MQTT";
static esp_mqtt_client_handle_t client = nullptr;

static void mqtt_event_handler(void* handler_args, esp_event_base_t base,
                               int32_t event_id, void* event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    switch (event->event_id) {
        case MQTT_EVENT_DATA: {
            char topic[64]; 
            snprintf(topic, sizeof(topic), "%.*s", event->topic_len, event->topic);
            char msg[16];
            snprintf(msg, sizeof(msg), "%.*s", event->data_len, event->data);

            int relay_id = 0;
            if (strncmp(topic, "home/relay/1", 12) == 0) relay_id = 1;
            else if (strncmp(topic, "home/relay/2", 12) == 0) relay_id = 2;
            else if (strncmp(topic, "home/relay/3", 12) == 0) relay_id = 3;
            else if (strncmp(topic, "home/relay/4", 12) == 0) relay_id = 4;

            if (relay_id > 0) {
                bool turnOn = (strncmp(msg, "ON", 2) == 0);
                RelayController::set(relay_id, turnOn);
            }
            break;
        }
        default:
            break;
    }
}

void MQTTManager::start() {
    esp_mqtt_client_config_t cfg = {};
    cfg.broker.address.uri = MQTT_BROKER;
    cfg.credentials.client_id = "ESP32_HOME_1";         // moved inside credentials
    cfg.credentials.username = MQTT_USERNAME;
    cfg.credentials.authentication.password = MQTT_PASSWORD;

    client = esp_mqtt_client_init(&cfg);
    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_event_handler, nullptr);
    esp_mqtt_client_start(client);

    esp_mqtt_client_subscribe(client, "home/relay/1", 0);
    esp_mqtt_client_subscribe(client, "home/relay/2", 0);
    esp_mqtt_client_subscribe(client, "home/relay/3", 0);
    esp_mqtt_client_subscribe(client, "home/relay/4", 0);

    ESP_LOGI(TAG, "MQTT connected to HiveMQ and subscribed to relay topics");
}
