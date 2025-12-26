idf_component_register(
    SRCS "app_main.cpp"
    INCLUDE_DIRS "."
    REQUIRES driver esp_wifi mqtt esp_http_server relay_controller web_server wifi_manager mqtt_manager
)