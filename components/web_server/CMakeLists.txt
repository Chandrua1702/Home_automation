idf_component_register(SRCS "web_server.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES relay_controller esp_http_server)
