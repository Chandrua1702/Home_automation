#pragma once

#include "esp_http_server.h"
#include <string>

class WifiWebServer {
public:
    static void start();

private:
    static httpd_handle_t server;

    static esp_err_t index_get_handler(httpd_req_t *req);
    static esp_err_t connect_psk_post_handler(httpd_req_t *req);
    static esp_err_t disconnect_post_handler(httpd_req_t *req);
    static esp_err_t scan_wifi_get_handler(httpd_req_t *req);
    static esp_err_t wifi_status_get_handler(httpd_req_t *req);

    static std::string read_post_data(httpd_req_t *req);
};
