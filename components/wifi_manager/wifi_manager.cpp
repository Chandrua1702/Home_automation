#include "wifi_manager.hpp"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <string.h>

static const char *TAG = "WIFI_MANAGER";
static bool connected = false;

/* ===================== NVS ===================== */

static void save_wifi(const char *ssid, const char *pass)
{
    nvs_handle_t nvs;
    ESP_ERROR_CHECK(nvs_open("wifi", NVS_READWRITE, &nvs));
    nvs_set_str(nvs, "ssid", ssid);
    nvs_set_str(nvs, "pass", pass);
    nvs_commit(nvs);
    nvs_close(nvs);
    ESP_LOGI(TAG, "WiFi credentials saved");
}

static bool load_wifi(char *ssid, char *pass)
{
    nvs_handle_t nvs;
    size_t len;

    if (nvs_open("wifi", NVS_READONLY, &nvs) != ESP_OK) {
        ESP_LOGW(TAG, "No WiFi credentials found");
        return false;
    }

    len = 32;
    if (nvs_get_str(nvs, "ssid", ssid, &len) != ESP_OK) return false;
    len = 64;
    if (nvs_get_str(nvs, "pass", pass, &len) != ESP_OK) return false;

    nvs_close(nvs);
    return true;
}

/* ===================== EVENTS ===================== */

static void wifi_event(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
        connected = false;
        auto *d = (wifi_event_sta_disconnected_t *)data;
        ESP_LOGW(TAG, "Disconnected (reason=%d), retrying", d->reason);
        esp_wifi_connect();
    }

    if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        connected = true;
        ESP_LOGI(TAG, "Got IP address, internet connected");
    }
}

/* ===================== API ===================== */

void WifiManager::init()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event, NULL));

    wifi_config_t ap_cfg = {};
    strcpy((char *)ap_cfg.ap.ssid, "ESP32-SETUP");
    ap_cfg.ap.ssid_len = strlen("ESP32-SETUP");
    ap_cfg.ap.channel = 1;
    ap_cfg.ap.max_connection = 4;
    ap_cfg.ap.authmode = WIFI_AUTH_OPEN;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_cfg));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi initialized (AP + STA)");
}

bool WifiManager::auto_connect()
{
    char ssid[32], pass[64];
    if (!load_wifi(ssid, pass)) return false;

    ESP_LOGI(TAG, "Auto-connecting to %s", ssid);
    return connect(ssid, pass) == ESP_OK;
}

esp_err_t WifiManager::connect(const char *ssid, const char *password)
{
    wifi_config_t cfg = {};
    strcpy((char *)cfg.sta.ssid, ssid);
    strcpy((char *)cfg.sta.password, password);

    save_wifi(ssid, password);
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &cfg));
    ESP_ERROR_CHECK(esp_wifi_connect());

    ESP_LOGI(TAG, "Connecting to WiFi: %s", ssid);
    return ESP_OK;
}

bool WifiManager::is_connected()
{
    return connected;
}
