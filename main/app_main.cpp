#include "wifi_manager.hpp"
#include "wifi_web_server.hpp"

extern "C" void app_main() {
    WifiManager::init();
    WifiWebServer::start();
}
