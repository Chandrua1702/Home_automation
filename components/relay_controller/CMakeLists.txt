idf_component_register(
    SRCS "relay_controller.cpp"
    INCLUDE_DIRS "." "../common"   # Add the folder where app_config.hpp is located
    REQUIRES driver
)
