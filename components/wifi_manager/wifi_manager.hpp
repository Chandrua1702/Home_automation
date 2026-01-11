#pragma once
#include "esp_err.h"
#include <string>
#include <vector>

struct APInfo {
    std::string ssid;
    int8_t rssi;
};

class WifiManager {
public:
    // Initialize Wi-Fi (AP+STA mode)
    static void init();

    // Scan available Wi-Fi networks
    static std::vector<APInfo> scan_networks();

    // Connect to Wi-Fi with given credentials
    static esp_err_t connect(const std::string &ssid, const std::string &password);

    // Check if currently connected to Wi-Fi
    static bool is_connected();

    // Save/load credentials to NVS
    static void save_credentials(const std::string &ssid, const std::string &password);
    static bool load_credentials(std::string &ssid, std::string &password);
};
