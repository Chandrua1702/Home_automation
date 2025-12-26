#pragma once
#include <stdbool.h>
#include "driver/gpio.h"
#include "app_config.hpp"
#include "esp_err.h"

class RelayController {
public:
    static void init();
    static void set(int relayNo, bool on);  // renamed
};
