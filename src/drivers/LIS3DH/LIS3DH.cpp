// LIS3DH (Accelerometer) Driver

#include <stdio.h>
#include <vector>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file

#include "hardware/i2c.h"
#include "drivers/logging/logging.h"

#include "drivers/Board/Board.h"

#include "LIS3DH.h"

// --- LIS3DH Driver Class Functions ---

void accelDriver::accelInit() {
    // Initialize the I2C interface for the LIS3DH accelerometer
    i2c_init(I2C_INSTANCE, 400 * 1000); // 400 kHz I2C speed
    gpio_set_function(ACCEL_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(ACCEL_SCL_PIN, GPIO_FUNC_I2C);

    // read the WHO_AM_I register to verify communication
    uint8_t waiOut; // Allocate variable for the output buffer
    if (1 == readRegister(WAI_REG, &waiOut, 1))
    {
        if (waiOut != 0x33) { // Check if the WHO_AM_I register returns the expected value
            log(LogLevel::ERROR, "LIS3DH accelerometer not detected");
            return; // Exit if the device is not detected
        }
        else {
            log(LogLevel::INFORMATION, "LIS3DH accelerometer detected successfully");
        }
    } // Read WHO_AM_I register
    
    // Configure the LIS3DH accelerometer CTRL_REG1
    if (!writeRegister(CTRL_REG1, 0x77)) { 
        log(LogLevel::ERROR, "Failed to configure LIS3DH accelerometer CTRL_REG1");
        return; // Exit if the configuration fails
    }

    // Configure the LIS3DH accelerometer CTRL_REG4
    if (!writeRegister(CTRL_REG4, 0x00)) { 
        log(LogLevel::ERROR, "Failed to configure LIS3DH accelerometer CTRL_REG4");
        return; // Exit if the configuration fails
    }

    // Configure the LIS3DH accelerometer TEMP_CFG_REG
    if (!writeRegister(TEMP_CFG_REG, 0x00)) { 
        log(LogLevel::ERROR, "Failed to configure LIS3DH accelerometer TEMP_CFG_REG");
        return; // Exit if the configuration fails
    }
}

bool accelDriver::writeRegister(uint8_t reg, uint8_t data) {
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = data;

    // Write data to the specified register of the LIS3DH
    int bytes_written = i2c_write_blocking(I2C_INSTANCE, I2C_ADDRESS, buf, 2, false);
    if (bytes_written != 2) {
        log(LogLevel::ERROR, "Failed to write to LIS3DH accelerometer");
        return false; // Return false if the write operation failed
    }
    return true; // Return true if the write operation was successful
}

bool accelDriver::readRegister(uint8_t reg, uint8_t *data, size_t length = 1) {
    if (1 != i2c_write_blocking(I2C_INSTANCE, I2C_ADDRESS, &reg, 1, true)) {
        // You need to pass the pointer to the register address because
        // i2c_write_blocking expects a pointer to a buffer of data.
        log(LogLevel::ERROR, "lis3dh::read_registers: Failed to select register address.");
        return false;
    }

    // Now read the data
    int bytes_read = i2c_read_blocking(I2C_INSTANCE, I2C_ADDRESS, data, length, false);
    if (bytes_read != length) {
        log(LogLevel::ERROR, "lis3dh::read_registers: Failed to read data.");
        return false;
    }

    return true; // Return true if the read operation was successful
}

void accelDriver::readAccelerometer(int16_t &x, int16_t &y, int16_t &z) {
    uint8_t data[6]; // Buffer to hold the accelerometer data

    // Read 6 bytes of data from the LIS3DH accelerometer
    if (!readRegister(READ_X_L, data, 6)) {
        log(LogLevel::ERROR, "Failed to read accelerometer data");
        return; // Exit if the read operation fails
    }

    // Convert the raw data to 16-bit integers
    x = (int16_t)((data[1] << 8) | data[0]);
    y = (int16_t)((data[3] << 8) | data[2]);
    z = (int16_t)((data[5] << 8) | data[4]);
}

float accelDriver::convertToGs(int16_t rawValue) {
    // Convert the raw accelerometer value to Gs (gravitational units)
    // 1LSB = 16mGs, therefore gs = rawValue / 16000.0f
    // This conversion assumes the LIS3DH is configured for ±2g full scale
    return (float)rawValue / 16000.0f;
}
