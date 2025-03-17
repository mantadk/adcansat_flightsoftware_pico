#include "umbrella.hpp"

bool isUpsideDown() {                             //Is the sensor upside down?
    return !gpio_get(UPSIDE_DOWN_SENSOR);            // Read and return the state
}

//read BME 280 sensor data via I2C

void initBME280() {
    i2c_init(i2c0, 100 * 1000);                     // Initialize I2C0 at 100kHz
    gpio_set_function(0, GPIO_FUNC_I2C);             // Set GPIO0 to I2C0 SDA
    gpio_set_function(1, GPIO_FUNC_I2C);             // Set GPIO1 to I2C0 SCL
    gpio_pull_up(0);                                 // Pull up GPIO0
    gpio_pull_up(1);                                 // Pull up GPIO1
}

BME280Data readBME280() {
    uint8_t data[8];                                // Create an array to store the data
    i2c_write_blocking(i2c0, 0x76, data, 1, false);  // Write 0x76 to the sensor
    i2c_read_blocking(i2c0, 0x76, data, 8, false);   // Read 8 bytes from the sensor

    BME280Data bmeData;                             // Create a BME280Data struct
    bmeData.temperature = (float)((data[3] << 12) | (data[4] << 4) | (data[5] >> 4)) / 16.0f; // Calculate temperature
    bmeData.pressure = (float)((data[0] << 12) | (data[1] << 4) | (data[2] >> 4)) / 100.0f; // Calculate pressure
    bmeData.humidity = (float)((data[6] << 8) | data[7]) / 1024.0f; // Calculate humidity
    return bmeData;                                 // Return the data
}