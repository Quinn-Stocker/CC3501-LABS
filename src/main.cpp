#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"
#include "drivers/LEDs/LEDs.h"
#include "drivers/Board/Board.h"
#include "drivers/LIS3DH/LIS3DH.h"

int main()
{
    stdio_init_all();

    // initialize the LEDController
    // LEDController ledController; // Initialize with Default Number of LEDs (12), to change this,
    // pass the desired number to the constructor in LEDController ledController(num_leds) format.
    // For example, to initialize with 6 LEDs, use LEDController ledController(6);
    // ledController.initLEDs();
    // Initialize the accelerometer driver
    accelDriver accelerometer;
    accelerometer.accelInit();


    for (;;) {
        // Test the log system
        log(LogLevel::INFORMATION, "Hello world");

        // ledController.getLED(0).setColor(255, 0, 0); // Set first LED to red
        // ledController.getLED(1).setColor(0, 255, 0); // Set second LED to green
        // ledController.getLED(2).setColor(0, 0, 255); // Set third LED to blue
        // ledController.setLEDGroup({5, 9, 11}, 255, 0, 255); // Set LEDs 5, 9, and 11 to magenta
        // ledController.updateLEDs(); // Update the LEDs to reflect the changes
        // sleep_ms(1000); // Wait for 1 second before the next update
        // ledController.getLED(7).setColor(255, 255, 0); // Set the 8th LED to yellow
        // ledController.updateLEDs(); // Update the LEDs to reflect the changes
        // sleep_ms(1000); // Wait for 1 second before the next update
        // ledController.resetLEDs(); // Reset all LEDs to off state
        // ledController.updateLEDs(); // Update the LEDs to reflect the reset

        int16_t x, y, z;
        accelerometer.readAccelerometer(x, y, z); // Read accelerometer data
        float xGs = accelerometer.convertToGs(x); // Convert raw X-axis data to Gs
        float yGs = accelerometer.convertToGs(y); // Convert raw Y-axis data to Gs
        float zGs = accelerometer.convertToGs(z); // Convert raw Z-axis data to Gs
        printf("Accelerometer Data: X = %.2f G, Y = %.2f G, Z = %.2f G\n", xGs, yGs, zGs);
    }

    return 0;
}
