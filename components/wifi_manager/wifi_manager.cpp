#include "wifi_manager.hpp"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <cstring>

static const char *TAG = "WIFI_MANAGER";
static bool connected = false;

///////////////////////
// NVS Save/Load
///////////////////////
void WifiManager::save_credentials(const std::string &ssid, const std::string &password) {
    nvs_handle_t nvs;
    ESP_ERROR_CHECK(nvs_open("wifi", NVS_READWRITE, &nvs));
    nvs_set_str(nvs, "ssid", ssid.c_str());
    nvs_set_str(nvs, "pass", password.c_str());
    nvs_commit(nvs);
    nvs_close(nvs);
    ESP_LOGI(TAG, "Wi-Fi credentials saved");
}

bool WifiManager::load_credentials(std::string &ssid, std::string &password) {
    nvs_handle_t nvs;
    if (nvs_open("wifi", NVS_READONLY, &nvs) != ESP_OK) return false;

    char s[33] = {0}, p[65] = {0};
    size_t slen = sizeof(s), plen = sizeof(p);

    if (nvs_get_str(nvs, "ssid", s, &slen) != ESP_OK) { nvs_close(nvs); return false; }
    if (nvs_get_str(nvs, "pass", p, &plen) != ESP_OK) { nvs_close(nvs); return false; }

    ssid = std::string(s);
    password = std::string(p);
    nvs_close(nvs);
    return true;
}

///////////////////////
// Wi-Fi Event Handler
///////////////////////
static void wifi_event(void *arg, esp_event_base_t base, int32_t id, void *data) {
    if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
        connected = false;
        auto *d = (wifi_event_sta_disconnected_t *)data;
        ESP_LOGW(TAG, "Disconnected (reason=%d), retrying", d->reason);
        esp_wifi_connect();
    }
    if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        connected = true;
        ESP_LOGI(TAG, "Connected! Got IP address");
    }
}

///////////////////////
// Initialize Wi-Fi
///////////////////////
void WifiManager::init() {
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event, nullptr));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event, nullptr));

    wifi_config_t ap_cfg = {};
    strcpy((char *)ap_cfg.ap.ssid, "ESP32-SETUP");
    ap_cfg.ap.ssid_len = strlen("ESP32-SETUP");
    ap_cfg.ap.channel = 1;
    ap_cfg.ap.max_connection = 4;
    ap_cfg.ap.authmode = WIFI_AUTH_OPEN;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_cfg));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wi-Fi initialized (AP + STA)");
}

///////////////////////
// Scan networks
///////////////////////
std::vector<APInfo> WifiManager::scan_networks() {
    wifi_scan_config_t cfg = {};
    ESP_ERROR_CHECK(esp_wifi_scan_start(&cfg, true));

    uint16_t n = 20;
    wifi_ap_record_t aps[20];
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&n, aps));

    std::vector<APInfo> list;
    for (int i = 0; i < n; i++) {
        list.push_back({ std::string((char*)aps[i].ssid), aps[i].rssi });
    }
    return list;
}

///////////////////////
// Connect STA
///////////////////////
esp_err_t WifiManager::connect(const std::string &ssid, const std::string &password) {
    wifi_config_t cfg = {};
    strcpy((char *)cfg.sta.ssid, ssid.c_str());
    strcpy((char *)cfg.sta.password, password.c_str());

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &cfg));
    ESP_ERROR_CHECK(esp_wifi_connect());

    save_credentials(ssid, password);
    ESP_LOGI(TAG, "Connecting to Wi-Fi: %s", ssid.c_str());
    return ESP_OK;
}

bool WifiManager::is_connected() {
    return connected;
}
