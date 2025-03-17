#include "umbrella.hpp"

void init_gpio()
{
    gpio_init(LED);                            // Initialize LED pin
    gpio_set_dir(LED, GPIO_OUT);               // Set LED pin as output
    gpio_init(UPSIDE_DOWN_SENSOR);             // Initialize GPIO10
    gpio_set_dir(UPSIDE_DOWN_SENSOR, GPIO_IN); // Set as input
    // Initialize virtual serial pins
    gpio_init(VIRTUAL_DATA_LINE);
    gpio_set_dir(VIRTUAL_DATA_LINE, GPIO_OUT);
    gpio_init(VIRTUAL_CLOCK_LINE);
    gpio_set_dir(VIRTUAL_CLOCK_LINE, GPIO_OUT);
    gpio_init(VIRTUAL_ENABLE_LINE);
    gpio_set_dir(VIRTUAL_ENABLE_LINE, GPIO_IN);
    gpio_init(VIRTUAL_NOTIFY_LINE);
    gpio_set_dir(VIRTUAL_NOTIFY_LINE, GPIO_OUT);
    for (int i = 0; i < 10; i++)
    {
        // blink led
        gpio_put(LED, 1);
        sleep_ms(100);
        gpio_put(LED, 0);
        sleep_ms(100);
    }
}

void catch_error()
{
    while (true)
    {
        gpio_put(LED, 1); // Turn on the LED
        sleep_ms(500);    // Wait for 500ms
        gpio_put(LED, 0); // Turn off the LED
        sleep_ms(500);    // Wait for 500ms
    }
}

int main()
{
    init_gpio();
    while (true)
    {
        BME280Data data = readBME280();
        bool upsideDown = isUpsideDown();
        std::string datasummary = "ud:" + std::string(upsideDown ? "1;" : "0;");
        datasummary += "t:" + std::to_string(data.temperature) + ";";
        datasummary += "p:" + std::to_string(data.pressure) + ";";
        datasummary += "h:" + std::to_string(data.humidity) + ";";
        sendVUARTString(datasummary);
    }

    return 0;
}
