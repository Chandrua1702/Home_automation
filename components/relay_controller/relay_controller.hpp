#pragma once
#include "driver/gpio.h"
#include "app_config.hpp"

class RelayController {
public:
    static void init();
    static void set(int relay_id, bool state);
};
