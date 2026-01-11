#include "storage.hpp"
#include "nvs_flash.h"
#include "nvs.h"

#define WIFI_NS "wifi"

bool storage_save_wifi(const std::string& ssid, const std::string& pass)
{
    nvs_handle_t handle;
    if (nvs_open(WIFI_NS, NVS_READWRITE, &handle) != ESP_OK)
        return false;

    nvs_set_str(handle, "ssid", ssid.c_str());
    nvs_set_str(handle, "pass", pass.c_str());
    nvs_commit(handle);
    nvs_close(handle);
    return true;
}

bool storage_load_wifi(std::string& ssid, std::string& pass)
{
    nvs_handle_t handle;
    if (nvs_open(WIFI_NS, NVS_READONLY, &handle) != ESP_OK)
        return false;

    char ssid_buf[64], pass_buf[64];
    size_t len1 = sizeof(ssid_buf), len2 = sizeof(pass_buf);

    if (nvs_get_str(handle, "ssid", ssid_buf, &len1) != ESP_OK ||
        nvs_get_str(handle, "pass", pass_buf, &len2) != ESP_OK)
    {
        nvs_close(handle);
        return false;
    }

    ssid = ssid_buf;
    pass = pass_buf;
    nvs_close(handle);
    return true;
}

void storage_clear_wifi()
{
    nvs_handle_t handle;
    if (nvs_open(WIFI_NS, NVS_READWRITE, &handle) == ESP_OK)
    {
        nvs_erase_all(handle);
        nvs_commit(handle);
        nvs_close(handle);
    }
}
