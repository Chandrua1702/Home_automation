idf_component_register(SRCS "wifi_web_server.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES relay_controller esp_http_server wifi_manager nvs_flash)
