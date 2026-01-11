idf_component_register(SRCS "storage.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES relay_controller esp_http_server nvs_flash)

