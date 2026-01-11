#include "wifi_manager.hpp"

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

static const char* TAG = "WIFI_MANAGER";

bool WifiManager::connected = false;

bool WifiManager::has_internet()
{
    struct addrinfo hints = {};
    struct addrinfo *res = nullptr;

    hints.ai_family = AF_INET;

    int err = getaddrinfo("google.com", nullptr, &hints, &res);

    if (err != 0 || res == nullptr) {
        ESP_LOGW(TAG, "Internet check FAILED (DNS)");
        return false;
    }

    freeaddrinfo(res);
    ESP_LOGI(TAG, "Internet check OK");
    return true;
}

void WifiManager::wifi_event_handler(
    void*,
    esp_event_base_t event_base,
    int32_t event_id,
    void*
)
{
    if (event_base == WIFI_EVENT &&
        event_id == WIFI_EVENT_STA_DISCONNECTED) {
        connected = false;
        ESP_LOGW(TAG, "WiFi disconnected");
    }

    if (event_base == IP_EVENT &&
        event_id == IP_EVENT_STA_GOT_IP) {
        connected = true;
        ESP_LOGI(TAG, "WiFi connected (IP obtained)");
    }
}

void WifiManager::init()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &WifiManager::wifi_event_handler,
        nullptr));

    ESP_ERROR_CHECK(esp_event_handler_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &WifiManager::wifi_event_handler,
        nullptr));

    wifi_config_t ap_cfg{};
    strcpy((char*)ap_cfg.ap.ssid, "Santrow-Network-Technologies");
    ap_cfg.ap.authmode = WIFI_AUTH_OPEN;
    ap_cfg.ap.max_connection = 4;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_cfg));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi initialized (AP + STA)");
}

std::vector<APInfo> WifiManager::scan_networks()
{
    wifi_scan_config_t scan_cfg{};
    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_cfg, true));

    uint16_t count = 20;
    wifi_ap_record_t records[20];
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&count, records));

    std::vector<APInfo> result;
    for (int i = 0; i < count; i++) {
        result.push_back({(char*)records[i].ssid, records[i].rssi});
    }
    return result;
}

esp_err_t WifiManager::connect(const std::string& ssid,
                               const std::string& password)
{
    wifi_config_t sta_cfg{};
    strcpy((char*)sta_cfg.sta.ssid, ssid.c_str());
    strcpy((char*)sta_cfg.sta.password, password.c_str());

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_cfg));
    ESP_ERROR_CHECK(esp_wifi_connect());

    ESP_LOGI(TAG, "Connecting to SSID: %s", ssid.c_str());
    return ESP_OK;
}

void WifiManager::disconnect()
{
    esp_wifi_disconnect();
    connected = false;
    ESP_LOGI(TAG, "WiFi disconnected by user");
}

bool WifiManager::is_connected()
{
    return connected;
}
