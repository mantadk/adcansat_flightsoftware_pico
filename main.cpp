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
    sendVUARTString("GPIO init done");
    i2c_inst_t *i2c = i2c0; 
    int sda_pin = 14;
    int scl_pin = 15;
    int i2c_baudrate = 100000;
    i2c_init(i2c, i2c_baudrate);
    sendVUARTString("I2C init done");
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);
    init_bme280(i2c);
    sendVUARTString("BME280 init done");
    bme280_calib_data_temp calib = Read_Temperature_Calibration_Data(i2c);
    sendVUARTString("Temperature calibration done");
    while (true)
    {
        long signed int raw = Read_Temperature(i2c);
        long signed int temp = Compensate_Temperature(raw, calib);
        std::stringstream ss;
        ss << "Temperature: " << temp << " C";
        std::string datasummary = ss.str();
        sendVUARTString(datasummary);
        sleep_ms(100);
    }

    return 0;
}
