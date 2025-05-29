// LED Driver

#include <stdio.h>
#include <vector>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "LEDs.h"
#include "drivers/Board/Board.h"

// --- LEDs Driver Class Functions ---

// - LED class functions -

// Constructor to initialize LED [led_num] with default color values
LED::LED(int led_num) : led_num(led_num), red(0), green(0), blue(0) {}

// Set the color of the LED
// Accepts RGB values in the range 0-255
void LED::setColor(uint8_t r, uint8_t g, uint8_t b) {
    red = r;
    green = g;
    blue = b;
}

// formatColor() returns the color in a format suitable for WS2812
// The format is a 32-bit integer where the first byte is red, second byte is green,
// and third byte is blue.
uint32_t LED::formatColor() const {
    return (red << 24) | (green << 16) | (blue << 8);
}

// - LEDController class functions -

// Constructor to initialize the LEDController with a specified number of LEDs
LEDController::LEDController(int num_leds) : LEDs() {
    for (int i = 0; i < num_leds; ++i) {
        LEDs.emplace_back(i);
    }
}

// initialize LEDs
void LEDController::initLEDs() {
    // Initialise PIO0 to control the LED chain
    uint pio_program_offset = pio_add_program(pio0, &ws2812_program);
    // Set up the WS2812 program with the specified parameters
    ws2812_program_init(pio0, 0, pio_program_offset, LED_PIN, 800000, false);
}

// getLED() returns a reference to the LED at the specified index
// This allows direct manipulation of the LED's properties
LED& LEDController::getLED(int index) {
    return LEDs.at(index);
}

// setLEDGroup() sets a group of LEDs to the specified color
// This function takes an array of indices and sets the corresponding LEDs to the given RGB color.
void LEDController::setLEDGroup(const std::vector<int>& indices, uint8_t r, uint8_t g, uint8_t b) {
    for (int index : indices) {
        if (index >= 0 && index < LEDs.size()) {
            LEDs[index].setColor(r, g, b); // Set the color of the specified LED
        }
    }
}

// resetLEDs() resets all LEDs to off state
void LEDController::resetLEDs() {
    for (auto& led : LEDs) {
        led.setColor(0, 0, 0); // Set each LED to black (off)
    }
}

// Update the LEDs by sending their color data to the WS2812 chain
// This function assumes that the PIO and WS2812 program are already initialized
void LEDController::updateLEDs() {
    for (const auto& led : LEDs) {
        uint32_t color = led.formatColor();
        pio_sm_put_blocking(pio0, 0, color);
    }
    sleep_ms(500); // Delay to allow the LEDs to update
}