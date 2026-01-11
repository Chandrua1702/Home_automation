#include "wifi_web_server.hpp"
#include "wifi_manager.hpp"
#include "esp_log.h"
#include <string>

static const char *TAG = "WIFI_WEB_SERVER";
httpd_handle_t WifiWebServer::server = nullptr;

/* Embedded HTML symbols (filename only!) */
extern const unsigned char _binary_index_html_start[];
extern const unsigned char _binary_index_html_end[];

std::string WifiWebServer::read_post_data(httpd_req_t *req)
{
    std::string body(req->content_len, '\0');
    httpd_req_recv(req, body.data(), body.size());
    return body;
}

esp_err_t WifiWebServer::index_get_handler(httpd_req_t *req)
{
    size_t len = _binary_index_html_end - _binary_index_html_start;
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char*)_binary_index_html_start, len);
    return ESP_OK;
}

esp_err_t WifiWebServer::connect_psk_post_handler(httpd_req_t *req)
{
    auto data = read_post_data(req);

    auto s = data.find("\"ssid\":\"");
    auto p = data.find("\"password\":\"");
    if (s == std::string::npos || p == std::string::npos)
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");

    std::string ssid = data.substr(s+8, data.find('"', s+8)-(s+8));
    std::string pass = data.substr(p+12, data.find('"', p+12)-(p+12));

    WifiManager::connect(ssid, pass);
    httpd_resp_sendstr(req, "{\"status\":\"connecting\"}");
    return ESP_OK;
}

esp_err_t WifiWebServer::disconnect_post_handler(httpd_req_t *req)
{
    WifiManager::disconnect();
    httpd_resp_sendstr(req, "{\"status\":\"disconnected\"}");
    return ESP_OK;
}

esp_err_t WifiWebServer::scan_wifi_get_handler(httpd_req_t *req)
{
    auto aps = WifiManager::scan_networks();
    std::string json = "[";

    for (size_t i=0;i<aps.size();i++) {
        json += "{\"ssid\":\""+aps[i].ssid+"\",\"rssi\":"+std::to_string(aps[i].rssi)+"}";
        if (i+1<aps.size()) json += ",";
    }
    json += "]";

    httpd_resp_set_type(req,"application/json");
    httpd_resp_send(req,json.c_str(),json.size());
    return ESP_OK;
}

esp_err_t WifiWebServer::wifi_status_get_handler(httpd_req_t *req)
{
    std::string s = WifiManager::is_connected() ? "connected":"disconnected";
    httpd_resp_sendstr(req, ("{\"status\":\""+s+"\"}").c_str());
    return ESP_OK;
}

static esp_err_t internet_status_get_handler(httpd_req_t *req)
{
    bool internet = WifiManager::has_internet();

    char resp[64];
    snprintf(resp, sizeof(resp),
             "{\"internet\": %s}",
             internet ? "true" : "false");

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

void WifiWebServer::start()
{
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&server, &cfg) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        return;
    }

    static httpd_uri_t uri_index = {
        .uri      = "/",
        .method   = HTTP_GET,
        .handler  = index_get_handler,
        .user_ctx = nullptr
    };

    static httpd_uri_t uri_connect = {
        .uri      = "/connect_psk",
        .method   = HTTP_POST,
        .handler  = connect_psk_post_handler,
        .user_ctx = nullptr
    };

    static httpd_uri_t uri_disconnect = {
        .uri      = "/disconnect",
        .method   = HTTP_POST,
        .handler  = disconnect_post_handler,
        .user_ctx = nullptr
    };

    static httpd_uri_t uri_scan = {
        .uri      = "/scan_wifi",
        .method   = HTTP_GET,
        .handler  = scan_wifi_get_handler,
        .user_ctx = nullptr
    };

    static httpd_uri_t uri_status = {
        .uri      = "/wifi_status",
        .method   = HTTP_GET,
        .handler  = wifi_status_get_handler,
        .user_ctx = nullptr
    };

    static httpd_uri_t uri_internet = {
        .uri      = "/api/internet",
        .method   = HTTP_GET,
        .handler  = internet_status_get_handler,
        .user_ctx = nullptr
    };

    httpd_register_uri_handler(server, &uri_index);
    httpd_register_uri_handler(server, &uri_connect);
    httpd_register_uri_handler(server, &uri_disconnect);
    httpd_register_uri_handler(server, &uri_scan);
    httpd_register_uri_handler(server, &uri_status);
    httpd_register_uri_handler(server, &uri_internet);

    ESP_LOGI(TAG, "WiFi Web Server started");
}
