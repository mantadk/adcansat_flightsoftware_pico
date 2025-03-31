#include "umbrella.hpp"

void init_gpio()
{
    gpio_init(LED);                            // Initialize LED pin
    gpio_set_dir(LED, GPIO_OUT);               // Set LED pin as output
    // Initialize virtual serial pins
    gpio_init(VIRTUAL_DATA_LINE);
    gpio_set_dir(VIRTUAL_DATA_LINE, GPIO_OUT);
    gpio_init(VIRTUAL_CLOCK_LINE);
    gpio_set_dir(VIRTUAL_CLOCK_LINE, GPIO_OUT);
    gpio_init(VIRTUAL_ENABLE_LINE);
    gpio_set_dir(VIRTUAL_ENABLE_LINE, GPIO_IN);
    gpio_init(VIRTUAL_NOTIFY_LINE);
    gpio_set_dir(VIRTUAL_NOTIFY_LINE, GPIO_OUT);
    for (int i = 0; i < 30; i++)
    {
        // blink led
        gpio_put(LED, 1);
        sleep_ms(100);
        gpio_put(LED, 0);
        sleep_ms(100);
    }
}

void init_serial()
{
    stdio_init_all();
}

void printserial(std::string line)
{
    printf("%s\n", line.c_str());
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
    init_serial();
    printserial("Pico GPIO init done");
    printserial("Pico Serial init done");
    gpio_put(LED, 1);
    sendVUARTString("Pico GPIO init done");
    gpio_put(LED, 0);
    printserial("Vuart String sent");
    i2c_inst_t *i2c = i2c0; 
    int sda_pin = 14;
    int scl_pin = 15;
    int i2c_baudrate = 100000;
    printserial("I2C init start");
    i2c_init(i2c, i2c_baudrate);
    printserial("I2C init done");
    sendVUARTString("I2C init done");
    printserial("vuart string sent");
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);
    printserial("I2C SDA and SCL pins set to I2C function");
    init_bme280(i2c);
    sendVUARTString("BME280 init done");
    printserial("BME280 init done");
    bme280_calib_data_temp calib = Read_Temperature_Calibration_Data(i2c);
    sendVUARTString("Temperature calibration done");
    printserial("Temperature calibration done");
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
