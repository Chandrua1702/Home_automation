#include "mqtt_manager.hpp"
#include "../common/app_config.hpp"
#include "../relay_controller/relay_controller.hpp"
#include "esp_log.h"
#include "mqtt_client.h"
#include <cstring>

static const char* TAG = "MQTT";
static esp_mqtt_client_handle_t client = nullptr;

// HiveMQ Cloud Root CA (ISRG Root X1)
static const char* root_ca = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

static void mqtt_event_handler(void* handler_args,
                               esp_event_base_t base,
                               int32_t event_id,
                               void* event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "✅ MQTT CONNECTED");
            esp_mqtt_client_subscribe(client, "home/esp32_001/relay/1/cmd", 0);
            esp_mqtt_client_subscribe(client, "home/esp32_001/relay/2/cmd", 0);
            esp_mqtt_client_subscribe(client, "home/esp32_001/relay/3/cmd", 0);
            esp_mqtt_client_subscribe(client, "home/esp32_001/relay/4/cmd", 0);
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
            if (strcmp(topic, "home/esp32_001/relay/1/cmd") == 0) relay = 1;
            else if (strcmp(topic, "home/esp32_001/relay/2/cmd") == 0) relay = 2;
            else if (strcmp(topic, "home/esp32_001/relay/3/cmd") == 0) relay = 3;
            else if (strcmp(topic, "home/esp32_001/relay/4/cmd") == 0) relay = 4;

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
    cfg.broker.address.uri = MQTT_BROKER;   // mqtts://...
    cfg.credentials.username = MQTT_USERNAME;
    cfg.credentials.authentication.password = MQTT_PASSWORD;
    cfg.credentials.client_id = MQTT_CLIENTID;
    cfg.broker.verification.certificate = root_ca;

    client = esp_mqtt_client_init(&cfg);
    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_event_handler, nullptr);

    esp_err_t err = esp_mqtt_client_start(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "🚀 MQTT client started");
    } else {
        ESP_LOGE(TAG, "❌ MQTT start failed: %s", esp_err_to_name(err));
    }
}