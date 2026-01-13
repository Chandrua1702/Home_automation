#include "relay_controller.hpp"

void RelayController::init() {
    gpio_config_t io_conf = {};
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL<<RELAY1_GPIO) |
                           (1ULL<<RELAY2_GPIO) |
                           (1ULL<<RELAY3_GPIO) |
                           (1ULL<<RELAY4_GPIO);
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    // Default OFF
    gpio_set_level((gpio_num_t)RELAY1_GPIO, 0);
    gpio_set_level((gpio_num_t)RELAY2_GPIO, 0);
    gpio_set_level((gpio_num_t)RELAY3_GPIO, 0);
    gpio_set_level((gpio_num_t)RELAY4_GPIO, 0);
}

void RelayController::set(int relayNo, bool state) {
    int gpio = 0;
    switch (relayNo) {
        case 1: gpio = RELAY1_GPIO; break;
        case 2: gpio = RELAY2_GPIO; break;
        case 3: gpio = RELAY3_GPIO; break;
        case 4: gpio = RELAY4_GPIO; break;
        default: return;
    }
    gpio_set_level((gpio_num_t)gpio, state ? 1 : 0);
}
