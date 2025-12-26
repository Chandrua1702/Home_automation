#include "relay_controller.hpp"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "RELAY";

// Make sure RELAYx_GPIO are defined in app_config.hpp
static const int RELAY_PINS[4] = {RELAY1_GPIO, RELAY2_GPIO, RELAY3_GPIO, RELAY4_GPIO};

void RelayController::init() {
    for (int i = 0; i < 4; i++) {
        gpio_reset_pin(static_cast<gpio_num_t>(RELAY_PINS[i]));
        gpio_set_direction(static_cast<gpio_num_t>(RELAY_PINS[i]), GPIO_MODE_OUTPUT);
        gpio_set_level(static_cast<gpio_num_t>(RELAY_PINS[i]), 0); // OFF
    }
    ESP_LOGI(TAG, "Relays initialized");
}

void RelayController::set(int relay_id, bool state) {
    if (relay_id < 0 || relay_id > 3) return;
    gpio_set_level(static_cast<gpio_num_t>(RELAY_PINS[relay_id]), state ? 1 : 0);
    ESP_LOGI(TAG, "Relay %d set to %d", relay_id + 1, state);
}
