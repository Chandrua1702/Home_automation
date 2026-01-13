#include "mqtt_manager.hpp"
#include "../common/app_config.hpp"
#include "../relay_controller/relay_controller.hpp"
#include "esp_log.h"
#include "mqtt_client.h"
#include <cstring>

static const char* TAG = "MQTT";
static esp_mqtt_client_handle_t client = nullptr;

static void mqtt_event_handler(void* handler_args,
                               esp_event_base_t base,
                               int32_t event_id,
                               void* event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch ((esp_mqtt_event_id_t)event_id) {

        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "✅ MQTT CONNECTED");
            esp_mqtt_client_subscribe(client, "home/relay/1", 0);
            esp_mqtt_client_subscribe(client, "home/relay/2", 0);
            esp_mqtt_client_subscribe(client, "home/relay/3", 0);
            esp_mqtt_client_subscribe(client, "home/relay/4", 0);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "❌ MQTT DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "📥 SUBSCRIBED msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_DATA: {
            char topic[64];
            char msg[16];

            snprintf(topic, sizeof(topic), "%.*s", event->topic_len, event->topic);
            snprintf(msg, sizeof(msg), "%.*s", event->data_len, event->data);

            ESP_LOGI(TAG, "📩 Topic: %s | Data: %s", topic, msg);

            int relay = 0;
            if (strcmp(topic, "home/relay/1") == 0) relay = 1;
            else if (strcmp(topic, "home/relay/2") == 0) relay = 2;
            else if (strcmp(topic, "home/relay/3") == 0) relay = 3;
            else if (strcmp(topic, "home/relay/4") == 0) relay = 4;

            if (relay > 0) {
                bool on = (strcmp(msg, "ON") == 0);
                RelayController::set(relay, on);
                ESP_LOGI(TAG, "🔌 Relay %d -> %s", relay, on ? "ON" : "OFF");
            }
            break;
        }

        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "🔥 MQTT ERROR");
            break;

        default:
            ESP_LOGI(TAG, "ℹ️ MQTT EVENT ID: %d", event_id);
            break;
    }
}

void MQTTManager::start()
{
    esp_mqtt_client_config_t cfg = {};

    cfg.broker.address.uri = MQTT_BROKER;   // mqtt://IP:1883
    cfg.credentials.username = MQTT_USERNAME;
    cfg.credentials.authentication.password = MQTT_PASSWORD;
    cfg.credentials.client_id = "ESP32_HOME_1";

    client = esp_mqtt_client_init(&cfg);

    // ✅ THIS IS THE FIX
    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_event_handler, nullptr);

    esp_err_t err = esp_mqtt_client_start(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "🚀 MQTT client started");
    } else {
        ESP_LOGE(TAG, "❌ MQTT start failed: %s", esp_err_to_name(err));
    }
}
