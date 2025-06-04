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
LED::LED(int led_num) : led_num(led_num), red(0), green(0), blue(0), applied_color({red,blue,green}) {}

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

// RGBColor() returns the RGB color as a vector
std::vector<uint8_t> LED::RGBColor() const {
    return {red, green, blue};
}

// applyColor() sets the applied color to the current RGB values
void LED::applyColor() {
    // Update the applied color with the current RGB values
    applied_color = {red, green, blue};
}

// getAppliedColor() returns the currently applied color
std::vector<uint8_t> LED::getAppliedColor() const {
    return applied_color;
}

// LEDNum() returns the LED number
int LED::LEDNum() const {
    return led_num;
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

// HSVtoRGB(int h, int s, int v) converts HSV values to RGB
std::vector<uint8_t> LEDController::HSVtoRGB(int h, int s, int v) const {
    float r, g, b;
    float f, p, q, t;
    int i;

    if (s == 0) {
        r = g = b = v / 255.0f; // Achromatic (grey)
    } else {
        h /= 60; // Sector 0 to 5
        i = static_cast<int>(h);
        f = h - i; // Factorial part
        p = v * (1 - s / 255.0f);
        q = v * (1 - f * s / 255.0f);
        t = v * (1 - (1 - f) * s / 255.0f);

        switch (i % 6) {
            case 0: r = v; g = t; b = p; break;
            case 1: r = q; g = v; b = p; break;
            case 2: r = p; g = v; b = t; break;
            case 3: r = p; g = q; b = v; break;
            case 4: r = t; g = p; b = v; break;
            case 5: r = v; g = p; b = q; break;
        }
    }

    return {static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b)};
}

// - LED status functions -

// count() returns the number of LEDs managed by this controller
int LEDController::count() const {
    return LEDs.size();
}

// getStatus(const std::vector<int>& indices) returns the status of the specified LEDs
std::vector<std::string> LEDController::getStatus(const std::vector<int>& indices) const {
    std::vector<std::string> status;
    for (int index : indices) {
        if (index >= 0 && index < LEDs.size()) {
            const LED& led = LEDs[index];
            status.push_back("LED " + std::to_string(index) + ": " + 
                             "R=" + std::to_string(led.formatColor() >> 24 & 0xFF) + ", " +
                             "G=" + std::to_string(led.formatColor() >> 16 & 0xFF) + ", " +
                             "B=" + std::to_string(led.formatColor() >> 8 & 0xFF));
        } else {
            status.push_back("LED " + std::to_string(index) + ": Out of range");
        }
    }
    return status;
}

// getAction(const std::vector<int>& indices) returns the action for the specified LEDs
std::vector<std::string> LEDController::getAction(const std::vector<int>& indices) const {
    std::vector<std::string> actions;

    for (int index : indices) {
        if (index >= 0 && index < LEDs.size()) {
            if (LEDs[index].RGBColor() != LEDs[index].getAppliedColor()) {
                actions.push_back("LED " + std::to_string(index) + ": Pending RGB(" +
                                  std::to_string(LEDs[index].RGBColor()[0]) + "," +
                                  std::to_string(LEDs[index].RGBColor()[1]) + "," +
                                  std::to_string(LEDs[index].RGBColor()[2]) + ")");
            } else {
                actions.push_back("LED " + std::to_string(index) + ": No actions on standby");
            }
        } else {
            actions.push_back("LED " + std::to_string(index) + ": Out of range");
        }
    }
    return actions;
}

// getSummary() returns a summary of the LEDs
std::string LEDController::getSummary() const {
    std::string summary = "LED Summary:\n";
    for (const auto& led : LEDs) {
        int led_num = led.LEDNum();
        summary += "LED " + std::to_string(led_num) + ": " +
                   "R=" + std::to_string(led.formatColor() >> 24 & 0xFF) + ", " +
                   "G=" + std::to_string(led.formatColor() >> 16 & 0xFF) + ", " +
                   "B=" + std::to_string(led.formatColor() >> 8 & 0xFF) + ";" +
                   getAction({led_num})[0] + "\n";
    }
    return summary;
}