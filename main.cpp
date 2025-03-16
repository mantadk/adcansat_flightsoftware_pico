#include "umbrella.hpp"

void init_gpio()
{
    gpio_init(LED);                                 // Initialize LED pin
    gpio_set_dir(LED, GPIO_OUT);                    // Set LED pin as output
    gpio_init(UPSIDE_DOWN_SENSOR);                  // Initialize GPIO10
    gpio_set_dir(UPSIDE_DOWN_SENSOR, GPIO_IN);      // Set as input
    // Initialize virtual serial pins
    gpio_init(VIRTUAL_DATA_LINE);
    gpio_set_dir(VIRTUAL_DATA_LINE, GPIO_OUT);
    gpio_init(VIRTUAL_CLOCK_LINE);
    gpio_set_dir(VIRTUAL_CLOCK_LINE, GPIO_OUT);
    gpio_init(VIRTUAL_ENABLE_LINE);
    gpio_set_dir(VIRTUAL_ENABLE_LINE, GPIO_IN);
    gpio_init(VIRTUAL_NOTIFY_LINE);
    gpio_set_dir(VIRTUAL_NOTIFY_LINE, GPIO_OUT);
}

int main() {
    init_gpio();
    while (true) {
        BME280Data data = readBME280();
        bool upsideDown = isUpsideDown();
        if (upsideDown) {
            gpio_put(LED, 1);
        } else {
            gpio_put(LED, 0);
        }
        std::string datasummary = "Temperature: " + std::to_string(data.temperature) + " Pressure: " + std::to_string(data.pressure) + " Humidity: " + std::to_string(data.humidity) + " Upside Down: " + std::to_string(upsideDown);
        int res = sendString(datasummary);
        if (res != 0) {
            return res;
        }
    }

    return 0;
}
