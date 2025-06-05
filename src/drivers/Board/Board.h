// Pin definitions for the board
#pragma once

// LED pin definitions
#define LED_PIN 14

// LIS3DH (Accelerometer) pin definitions
#define I2C_INSTANCE i2c0
#define ACCEL_SDA_PIN 16
#define ACCEL_SCL_PIN 17
#define I2C_ADDRESS 0x19

#define WAI_REG 0x0F // WHO_AM_I register address for LIS3DH
#define CTRL_REG1 0x20 // Control register 1 address for LIS3DH
#define CTRL_REG4 0x23 // Control register 4 address for LIS3DH
#define TEMP_CFG_REG 0x1F // Temperature configuration register address for LIS3DH

#define READ_X_L 0x28 // X-axis low byte register address for LIS3DH
#define READ_X_H 0x29 // X-axis high byte register address for LIS3DH
#define READ_Y_L 0x2A // Y-axis low byte register address for LIS3DH
#define READ_Y_H 0x2B // Y-axis high byte register address for LIS3DH
#define READ_Z_L 0x2C // Z-axis low byte register address for LIS3DH
#define READ_Z_H 0x2D // Z-axis high byte register address for LIS3DH