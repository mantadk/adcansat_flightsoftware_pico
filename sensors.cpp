#include "umbrella.hpp"

bool isUpsideDown() {                             //Is the sensor upside down?
    return !(gpio_get(UPSIDE_DOWN_SENSOR));            // Read and return the state
}

//read BME 280 sensor data via I2C

void initBME280() {
    i2c_init(i2c0, 100000);
    gpio_set_function(0, GPIO_FUNC_I2C);
    gpio_set_function(1, GPIO_FUNC_I2C);
    gpio_pull_up(0);
    gpio_pull_up(1);
    uint8_t reset_cmd[2] = {0xE0, 0xB6};
    i2c_write_blocking(i2c0, 0x76, reset_cmd, 2, false);
    sleep_ms(10);
}

BME280Data readBME280() {
    uint8_t reg = 0xF7, data[8];
    i2c_write_blocking(i2c0, 0x76, &reg, 1, true);
    i2c_read_blocking(i2c0, 0x76, data, 8, false);
    BME280Data bmeData;
    bmeData.temperature = ((data[3] << 12) | (data[4] << 4) | (data[5] >> 4)) / 16.0f;
    bmeData.pressure = ((data[0] << 12) | (data[1] << 4) | (data[2] >> 4)) / 100.0f;
    bmeData.humidity = ((data[6] << 8) | data[7]) / 1024.0f;
    return bmeData;
}
