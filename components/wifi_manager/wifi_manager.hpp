#pragma once
#include "esp_err.h"

class WifiManager {
public:
    static void init();
    static bool auto_connect();
    static esp_err_t connect(const char *ssid, const char *password);
    static bool is_connected();
};
