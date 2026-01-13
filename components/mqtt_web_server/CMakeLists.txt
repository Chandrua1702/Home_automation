idf_component_register(
    SRCS "mqtt_web_server.cpp"
    EMBED_FILES "mqtt_webserver.html"
    INCLUDE_DIRS "."
    PRIV_REQUIRES mqtt_manager relay_controller esp_http_server
)
