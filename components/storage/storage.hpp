#pragma once
#include <string>

bool storage_save_wifi(const std::string& ssid, const std::string& pass);
bool storage_load_wifi(std::string& ssid, std::string& pass);
void storage_clear_wifi();
