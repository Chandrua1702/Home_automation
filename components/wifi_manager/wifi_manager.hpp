#pragma once

#include <string>
#include <vector>

#include "esp_err.h"
#include "esp_event.h"   // ✅ REQUIRED for esp_event_base_t

struct APInfo {
    std::string ssid;
    int8_t rssi;
};

class WifiManager {
public:
    static void init();
    static std::vector<APInfo> scan_networks();
    static esp_err_t connect(const std::string& ssid,
                             const std::string& password);
    static void disconnect();
    static bool is_connected();
	static bool has_internet(); 

private:
    static bool connected;

    // ✅ EXACT ESP-IDF signature
    static void wifi_event_handler(
        void* handler_arg,
        esp_event_base_t event_base,
        int32_t event_id,
        void* event_data
    );
};
