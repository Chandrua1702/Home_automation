
#include "wifi_web_server.hpp"
#include "wifi_manager.hpp"

// Use C linkage for ESP-IDF C headers to avoid C++ name mangling surprises
extern "C" {
#include "esp_http_server.h"
#include "esp_wifi.h"
#include "esp_log.h"
}
#include <cstring>  // for std::strlen, std::strcat if you keep C-string ops

static const char *TAG = "WIFI_WEB";

/* ===================== HTML ===================== */

static const char HTML_PAGE[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 WiFi Setup</title>
<style>
body{margin:0;height:100vh;display:flex;align-items:center;justify-content:center;
background:linear-gradient(135deg,#1e3c72,#2a5298);font-family:Arial}
.card{background:white;width:90%;max-width:380px;border-radius:16px;
padding:24px;box-shadow:0 15px 40px rgba(0,0,0,.3);text-align:center}
h2{color:#2a5298}
select,input,button{width:100%;padding:12px;margin-top:12px;font-size:16px}
button{background:#2a5298;color:white;border:none;border-radius:8px}
.status{margin-top:12px;font-size:14px;color:#555}
</style>
</head>
<body>
<div class="card">
<h2>Connect to Wi-Fi</h2>
<select id="ssid"></select>
<input id="pass" type="password" placeholder="Wi-Fi Password">
<button id="btn" onclick="connect()">Connect</button>
<div class="status" id="status">Scanning networks...</div>
</div>

<script>
const status=document.getElementById("status");
const btn=document.getElementById("btn");

fetch('/scan').then(r=>r.json()).then(list=>{
 let s=document.getElementById("ssid");
 s.innerHTML="";
 list.forEach(n=>{
  let o=document.createElement("option");
  o.text=o.value=n;
  s.appendChild(o);
 });
 status.innerText="Select your Wi-Fi";
});

function connect(){
 btn.disabled=true;
 status.innerText="Connecting...";
 let ssid=document.getElementById("ssid").value;
 let pass=document.getElementById("pass").value;
 fetch(`/connect?ssid=${encodeURIComponent(ssid)}&pass=${encodeURIComponent(pass)}`)
 .then(()=>status.innerText="ESP32 is connecting…")
 .catch(()=>{status.innerText="Failed";btn.disabled=false});
}
</script>
</body>
</html>
)rawliteral";

/* ===================== HANDLERS ===================== */

static esp_err_t root_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Root page requested");
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, HTML_PAGE, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t scan_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "WiFi scan requested");

    // Synchronous scan; blocks until done
    wifi_scan_config_t cfg = {};  // default: active scan on current channel set
    esp_err_t err = esp_wifi_scan_start(&cfg, true);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Scan start failed: %d", err);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "scan failed");
        return err;
    }

    // Get number of APs, then fetch records
    uint16_t ap_count = 0;
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));

    wifi_ap_record_t *aps = nullptr;
    if (ap_count > 0) {
        aps = (wifi_ap_record_t *)malloc(sizeof(wifi_ap_record_t) * ap_count);
    }
    if (ap_count > 0 && !aps) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "oom");
        return ESP_ERR_NO_MEM;
    }

    if (ap_count > 0) {
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_count, aps));
    }

    httpd_resp_set_type(req, "application/json");

    // Stream JSON to avoid fixed-size buffers & overflow risk
    httpd_resp_sendstr_chunk(req, "[");
    for (uint16_t i = 0; i < ap_count; ++i) {
        const char *ssid = reinterpret_cast<const char *>(aps[i].ssid);
        // Minimal JSON string escaping (handles quotes & backslashes)
        // For production, consider cJSON or a small escaper.
        httpd_resp_sendstr_chunk(req, "\"");
        for (const char *p = ssid; *p; ++p) {
            if (*p == '\"' || *p == '\\') {
                httpd_resp_sendstr_chunk(req, "\\");
            }
            char ch[2] = { *p, 0 };
            httpd_resp_sendstr_chunk(req, ch);
        }
        httpd_resp_sendstr_chunk(req, "\"");
        if (i + 1 < ap_count) {
            httpd_resp_sendstr_chunk(req, ",");
        }
    }
    httpd_resp_sendstr_chunk(req, "]");
    httpd_resp_send_chunk(req, nullptr, 0);  // end chunked response

    if (aps) free(aps);
    ESP_LOGI(TAG, "Scan complete: %u networks", ap_count);
    return ESP_OK;
}

static esp_err_t connect_handler(httpd_req_t *req)
{
    // Determine required query buffer size
    size_t qlen = httpd_req_get_url_query_len(req);
    if (qlen == 0) {
        ESP_LOGE(TAG, "Missing query");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "missing query");
        return ESP_ERR_INVALID_ARG;
    }

    // Allocate buffer (+1 for NUL)
    char *query = (char *)malloc(qlen + 1);
    if (!query) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "oom");
        return ESP_ERR_NO_MEM;
    }

    httpd_req_get_url_query_str(req, query, qlen + 1);

    char ssid[33] = {0};  // SSID max length 32 bytes + NUL
    char pass[65] = {0};  // WPA2 passphrase max 64 + NUL

    (void)httpd_query_key_value(query, "ssid", ssid, sizeof(ssid));
    (void)httpd_query_key_value(query, "pass", pass, sizeof(pass));

    free(query);

    ESP_LOGI(TAG, "User selected SSID: %s", ssid);
    WifiManager::connect(ssid, pass);

    httpd_resp_sendstr(req, "OK");
    return ESP_OK;
}

/* ===================== START ===================== */

void WifiWebServer::start()
{
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = nullptr;

    ESP_ERROR_CHECK(httpd_start(&server, &cfg));

    // Persistent descriptors (static lifetime)
    static const httpd_uri_t ROOT_URI{
        .uri      = "/",
        .method   = HTTP_GET,
        .handler  = root_handler,
        .user_ctx = nullptr,
    };
    static const httpd_uri_t SCAN_URI{
        .uri      = "/scan",
        .method   = HTTP_GET,
        .handler  = scan_handler,
        .user_ctx = nullptr,
    };
    static const httpd_uri_t CONNECT_URI{
        .uri      = "/connect",
        .method   = HTTP_GET,
        .handler  = connect_handler,
        .user_ctx = nullptr,
    };

    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &ROOT_URI));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &SCAN_URI));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &CONNECT_URI));

    ESP_LOGI(TAG, "WiFi provisioning server started");
}