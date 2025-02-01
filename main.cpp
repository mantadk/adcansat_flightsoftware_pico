#include "pico/stdlib.h"

#define LED 25
#define UPSIDE_DOWN_SENSOR 10

#define VIRTUAL_DATA_LINE 4
#define VIRTUAL_CLOCK_LINE 5
#define VIRTUAL_ENABLE_LINE 6
#define VIRTUAL_NOTIFY_LINE 7

bool is_gpio10_high() {                             //Is the sensor upside down?
    return gpio_get(UPSIDE_DOWN_SENSOR);            // Read and return the state
}

void init_gpio()
{
    gpio_init(LED);                                 // Initialize LED pin
    gpio_set_dir(LED, GPIO_OUT);                    // Set LED pin as output
    gpio_init(UPSIDE_DOWN_SENSOR);                  // Initialize GPIO10
    gpio_set_dir(UPSIDE_DOWN_SENSOR, GPIO_IN);      // Set as input
}

int main() {
    init_gpio();

    while (true) {
        gpio_put(LED, is_gpio10_high());
    }

    return 0;
}
