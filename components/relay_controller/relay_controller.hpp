#pragma once
#include <stdbool.h>
#include "driver/gpio.h"
#include "../common/app_config.hpp"

class RelayController {
public:
    static void init();
    static void set(int relayNo, bool state);  // relayNo = 1-4
};
