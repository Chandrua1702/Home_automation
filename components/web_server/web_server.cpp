#include "web_server.hpp"
#include "relay_controller.hpp"
#include "esp_log.h"
#include "esp_http_server.h"
#include <string>
#include <sstream>

static const char* TAG = "WEB_SERVER";

static esp_err_t relay_post_handler(httpd_req_t *req){
    char buf[100];
    int ret = httpd_req_recv(req, buf, sizeof(buf));
    if (ret <= 0) return ESP_OK;

    buf[ret] = 0;
    int relay = -1, state = -1;

    sscanf(buf, "relay=%d&state=%d", &relay, &state);
    if (relay >= 0 && relay <=3 && (state==0 || state==1)) {
        RelayController::set(relay, state);
    }
    httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t root_get_handler(httpd_req_t *req){
    const char* resp = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<style>
html, body {
    height: 100%;
    margin: 0;
    font-family: Arial, sans-serif;
    background: linear-gradient(to bottom, #f0f8ff, #add8e6);
    display: flex;
    justify-content: center;
    align-items: center;
}
.container {
    text-align: center;
    background-color: rgba(255,255,255,0.9);
    padding: 40px;
    border-radius: 20px;
    box-shadow: 0 0 20px rgba(0,0,0,0.3);
}
h1 { color: #333366; margin-bottom: 30px; }
h2 { color: #222; margin: 20px 0 10px 0; }
.button { 
    padding: 15px 30px; 
    margin: 10px; 
    font-size:18px; 
    border:none; 
    border-radius:8px; 
    cursor:pointer;
    min-width: 100px;
}
.on { background-color: #4CAF50; color:white; }
.off { background-color: #f44336; color:white; }
.button:hover { opacity:0.8; }
</style>
<script>
function toggleRelay(id, state){
    fetch('/relay', {
        method: 'POST',
        body: 'relay=' + id + '&state=' + state
    }).then(()=> {
        document.getElementById('r'+id+'_on').style.backgroundColor = state ? '#4CAF50' : '#ddd';
        document.getElementById('r'+id+'_off').style.backgroundColor = state ? '#ddd' : '#f44336';
    });
}
</script>
</head>
<body>
<div class="container">
<h1>ESP32 Relay Control</h1>

<div>
    <h2>Relay 1</h2>
    <button id="r0_on" class="button on" onclick="toggleRelay(0,1)">ON</button>
    <button id="r0_off" class="button off" onclick="toggleRelay(0,0)">OFF</button>
</div>
<div>
    <h2>Relay 2</h2>
    <button id="r1_on" class="button on" onclick="toggleRelay(1,1)">ON</button>
    <button id="r1_off" class="button off" onclick="toggleRelay(1,0)">OFF</button>
</div>
<div>
    <h2>Relay 3</h2>
    <button id="r2_on" class="button on" onclick="toggleRelay(2,1)">ON</button>
    <button id="r2_off" class="button off" onclick="toggleRelay(2,0)">OFF</button>
</div>
<div>
    <h2>Relay 4</h2>
    <button id="r3_on" class="button on" onclick="toggleRelay(3,1)">ON</button>
    <button id="r3_off" class="button off" onclick="toggleRelay(3,0)">OFF</button>
</div>
</div>
</body>
</html>
)rawliteral";

    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

void WebServer::init() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = nullptr;
    httpd_start(&server, &config);

    httpd_uri_t root_uri = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = root_get_handler,
        .user_ctx  = nullptr
    };
    httpd_register_uri_handler(server, &root_uri);

    httpd_uri_t relay_uri = {
        .uri       = "/relay",
        .method    = HTTP_POST,
        .handler   = relay_post_handler,
        .user_ctx  = nullptr
    };
    httpd_register_uri_handler(server, &relay_uri);

    ESP_LOGI(TAG, "Web server started");
}
