idf_component_register(
    SRCS "mqtt_manager.cpp"
    INCLUDE_DIRS "."
    REQUIRES mqtt esp_wifi relay_controller driver esp_event  # Add relay_controller here
)
