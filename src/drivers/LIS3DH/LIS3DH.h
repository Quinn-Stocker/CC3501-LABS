#pragma once

#include <vector>
#include <cstdint>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"


class accelDriver {
    private:
        bool writeRegister(uint8_t reg, uint8_t data);

        bool readRegister(uint8_t reg, uint8_t *data, size_t length);
    public:
        void accelInit();

        void readAccelerometer(int16_t &x, int16_t &y, int16_t &z);

        float convertToGs(int16_t rawValue);
};
