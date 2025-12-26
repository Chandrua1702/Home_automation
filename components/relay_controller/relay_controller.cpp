#include "relay_controller.hpp"
#include "../common/app_config.hpp"
#include "driver/gpio.h"

void RelayController::init() {
    gpio_config_t io_conf = {};
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL<<RELAY1_GPIO) | (1ULL<<RELAY2_GPIO) |
                           (1ULL<<RELAY3_GPIO) | (1ULL<<RELAY4_GPIO);
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);
}

void RelayController::set(int relay_id, bool state) {
    int gpio = 0;
    switch(relay_id) {
        case 1: gpio = RELAY1_GPIO; break;
        case 2: gpio = RELAY2_GPIO; break;
        case 3: gpio = RELAY3_GPIO; break;
        case 4: gpio = RELAY4_GPIO; break;
        default: return;
    }
    gpio_set_level((gpio_num_t)gpio, state ? 1 : 0);
}
