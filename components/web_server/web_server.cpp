#include "web_server.hpp"
#include "relay_controller.hpp" // Include the relay controller header
#include "esp_log.h"
#include "esp_http_server.h"
#include <cstring>
#include <cstdlib>

static const char* TAG = "WEB_SERVER";

static const char* HTML_PAGE = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>ESP32 Home Automation</title>
<style>
body { display:flex; justify-content:center; align-items:center; height:100vh; background:#f0f0f0; font-family:Arial; }
.container { background:white; padding:30px; border-radius:15px; text-align:center; }
button { width:100px; height:40px; margin:5px; border:none; border-radius:10px; font-size:16px; cursor:pointer; }
.on { background:green; color:white; } .off { background:red; color:white; }
</style>
</head>
<body>
<div class="container">
<h2>ESP32 Relays</h2>
<div>Relay 1 <button class="on" onclick="fetch('/relay/1/on')">ON</button><button class="off" onclick="fetch('/relay/1/off')">OFF</button></div>
<div>Relay 2 <button class="on" onclick="fetch('/relay/2/on')">ON</button><button class="off" onclick="fetch('/relay/2/off')">OFF</button></div>
<div>Relay 3 <button class="on" onclick="fetch('/relay/3/on')">ON</button><button class="off" onclick="fetch('/relay/3/off')">OFF</button></div>
<div>Relay 4 <button class="on" onclick="fetch('/relay/4/on')">ON</button><button class="off" onclick="fetch('/relay/4/off')">OFF</button></div>
</div>
</body>
</html>
)rawliteral";

// Root page handler
static esp_err_t root_get_handler(httpd_req_t *req) {
    httpd_resp_send(req, HTML_PAGE, strlen(HTML_PAGE));
    return ESP_OK;
}

// Relay handler
static esp_err_t relay_handler(httpd_req_t *req) {
    const char* path = req->uri; // e.g., /relay/1/on
    int relay_id = path[7] - '0'; // get relay number
    bool state = (strcmp(&path[9], "on") == 0);

    // If RelayController::set is static
    RelayController::set(relay_id, state);

    // If RelayController::set is non-static, use:
    // static RelayController relay;
    // relay.set(relay_id, state);

    httpd_resp_send(req, "OK", 2);
    return ESP_OK;
}

void WebServer::init() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = nullptr;

    if (httpd_start(&server, &config) == ESP_OK) {
        // Root page
        static httpd_uri_t root = { "/", HTTP_GET, root_get_handler, nullptr };
        httpd_register_uri_handler(server, &root);

        // Relay URIs
        for (int i = 1; i <= 4; i++) {
            // ON URI
            static char path_on[16];
            snprintf(path_on, sizeof(path_on), "/relay/%d/on", i);
            static httpd_uri_t uri_on;
            uri_on.uri = path_on;
            uri_on.method = HTTP_GET;
            uri_on.handler = relay_handler;
            uri_on.user_ctx = nullptr;
            httpd_register_uri_handler(server, &uri_on);

            // OFF URI
            static char path_off[16];
            snprintf(path_off, sizeof(path_off), "/relay/%d/off", i);
            static httpd_uri_t uri_off;
            uri_off.uri = path_off;
            uri_off.method = HTTP_GET;
            uri_off.handler = relay_handler;
            uri_off.user_ctx = nullptr;
            httpd_register_uri_handler(server, &uri_off);
        }

        ESP_LOGI(TAG, "HTTP Server started");
    }
}
