#include "wifi_web_server.hpp"
#include "wifi_manager.hpp"
#include "url_decode.hpp"
#include "esp_log.h"
extern "C" {
#include "esp_http_server.h"
}

static const char* TAG = "WIFI_WEB";

// HTML page for Wi-Fi provisioning
static const char HTML_PAGE[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 Wi-Fi Setup</title>
<style>
body{margin:0;height:100vh;display:flex;align-items:center;justify-content:center;
background:linear-gradient(135deg,#1e3c72,#2a5298);font-family:Arial}
.card{background:white;width:90%;max-width:380px;border-radius:16px;
padding:24px;box-shadow:0 15px 40px rgba(0,0,0,.3);text-align:center}
h2{color:#2a5298} select,input,button{width:100%;padding:12px;margin-top:12px;font-size:16}
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
 list.forEach(n=>{ let o=document.createElement("option"); o.text=o.value=n; s.appendChild(o); });
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

// Root page handler
static esp_err_t root_handler(httpd_req_t* req) {
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, HTML_PAGE, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Scan handler
static esp_err_t scan_handler(httpd_req_t* req) {
    auto list = WifiManager::scan_networks();
    std::string json = "[";
    for(size_t i=0;i<list.size();i++){
        json += "\"" + list[i].ssid + "\"";
        if(i<list.size()-1) json += ",";
    }
    json += "]";
    httpd_resp_set_type(req,"application/json");
    httpd_resp_send(req,json.c_str(),json.length());
    return ESP_OK;
}

// Connect handler
static esp_err_t connect_handler(httpd_req_t* req) {
    size_t qlen = httpd_req_get_url_query_len(req);
    if (qlen == 0) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing query");
    return ESP_ERR_INVALID_ARG;
}

    char* query = (char*)malloc(qlen+1);
    httpd_req_get_url_query_str(req,query,qlen+1);

    char ssid[33]={0}, pass[65]={0};
    httpd_query_key_value(query,"ssid",ssid,sizeof(ssid));
    httpd_query_key_value(query,"pass",pass,sizeof(pass));
    free(query);

    WifiManager::connect(url_decode(ssid), url_decode(pass));
    httpd_resp_sendstr(req,"OK");
    return ESP_OK;
}

// Start the web server
void WifiWebServer::start() {
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = nullptr;
    ESP_ERROR_CHECK(httpd_start(&server,&cfg));

    static const httpd_uri_t ROOT_URI{"/",HTTP_GET,root_handler,nullptr};
    static const httpd_uri_t SCAN_URI{"/scan",HTTP_GET,scan_handler,nullptr};
    static const httpd_uri_t CONNECT_URI{"/connect",HTTP_GET,connect_handler,nullptr};

    ESP_ERROR_CHECK(httpd_register_uri_handler(server,&ROOT_URI));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server,&SCAN_URI));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server,&CONNECT_URI));

    ESP_LOGI(TAG,"Wi-Fi provisioning server started at 192.168.4.1");
}
