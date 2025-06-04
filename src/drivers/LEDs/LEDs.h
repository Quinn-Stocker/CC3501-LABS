#pragma once

// Include necessary libraries
#include <vector>
#include <string>

// -- LED Driver Classes --

// Single LED class to represent an individual LED
class LED {
    private:
        // LED number and its RGB color values
        int led_num;
        uint8_t red, green, blue;
        std::vector<uint8_t> applied_color;
    public:
        // Constructor to initialize LED [led_num]
        LED(int led_num);

        // Set the color of the LED
        // Accepts RGB values in the range 0-255
        void setColor(uint8_t r, uint8_t g, uint8_t b);

        // formatColor() returns the color in a format suitable for WS2812
        // The format is a 32-bit integer where the first byte is red, second byte is green,
        // and third byte is blue.
        // This is used to send the color data to the LED strip.
        uint32_t formatColor() const;

        // RGBColor() returns the RGB color as a vector
        std::vector<uint8_t> RGBColor() const;

        // applyColor() applies the current color to the LED
        void applyColor();

        // getAppliedColor() returns the currently applied color
        std::vector<uint8_t> getAppliedColor() const;

        // LEDNum() returns the LED number
        int LEDNum() const;
};

// LEDController class to manage a dynamic number of LEDs
class LEDController {
    private:
        // Default number of LEDs
        static constexpr int DEFAULT_NUM_LEDS = 12;

        // Vector to hold the state of each LED
        std::vector<LED> LEDs;

    public:
        // Constructor to initialize the LEDController with a specified number of LEDs
        LEDController(int num_leds = DEFAULT_NUM_LEDS);

        // initLEDs() initializes the functionality of the LEDs
        void initLEDs();

        // getLED() returns a reference to the LED at the specified index
        LED& getLED(int index);

        // setLEDGroup() sets a group of LEDs to the specified color
        void setLEDGroup(const std::vector<int>& indices, uint8_t r, uint8_t g, uint8_t b);

        // resetLEDs() resets all LEDs to off state
        void resetLEDs();

        // updateLEDs() updates the state of all LEDs
        void updateLEDs();

        // HSVtoRGB(int h, int s, int v) converts HSV values to RGB
        std::vector<uint8_t> HSVtoRGB(int h, int s, int v) const;

        // - LED status functions -

        // count() returns the number of LEDs managed by this controller
        int count() const;

        // getStatus(const std::vector<int>& indices) returns the status of the specified LEDs
        std::vector<std::string> getStatus(const std::vector<int>& indices) const;

        // getAction(const std::vector<int>& indices) returns the action for the specified LEDs
        std::vector<std::string> getAction(const std::vector<int>& indices) const;

        // getSummary() returns a summary of the LEDs
        std::string getSummary() const;
};
