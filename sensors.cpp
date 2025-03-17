#include "umbrella.hpp"

bool isUpsideDown() {                             //Is the sensor upside down?
    return !(gpio_get(UPSIDE_DOWN_SENSOR));            // Read and return the state
}

//read BME 280 sensor data via I2C

void initBME280() {
    i2c_init(I2C_PORT, 100 * 1000);  // Initialize I2C0 at 100kHz
    gpio_set_function(0, GPIO_FUNC_I2C);  // SDA
    gpio_set_function(1, GPIO_FUNC_I2C);  // SCL
    gpio_pull_up(0);
    gpio_pull_up(1);

    // Check if the device is responding
    uint8_t chip_id_reg = 0xD0;
    uint8_t chip_id;
    if (i2c_write_blocking(I2C_PORT, BME280_I2C_ADDR, &chip_id_reg, 1, true) != PICO_ERROR_GENERIC &&
        i2c_read_blocking(I2C_PORT, BME280_I2C_ADDR, &chip_id, 1, false) != PICO_ERROR_GENERIC) {
        if (chip_id != 0x60) {
            printf("BME280 not detected!\n");
        }
    } else {
        printf("I2C communication failed!\n");
    }

    // Soft reset the sensor
    uint8_t reset_cmd[2] = {0xE0, 0xB6};
    i2c_write_blocking(I2C_PORT, BME280_I2C_ADDR, reset_cmd, 2, false);
    sleep_ms(10);
}

BME280Data readBME280() {
    uint8_t reg = 0xF7;  // Start of pressure/temp/humidity data
    uint8_t data[8];

    // Write register address
    if (i2c_write_blocking(I2C_PORT, BME280_I2C_ADDR, &reg, 1, true) != PICO_ERROR_GENERIC) {
        // Read 8 bytes of data
        if (i2c_read_blocking(I2C_PORT, BME280_I2C_ADDR, data, 8, false) != PICO_ERROR_GENERIC) {
            // Extract raw values
            int32_t adc_p = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
            int32_t adc_t = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);
            int32_t adc_h = (data[6] << 8) | data[7];

            // Placeholder for calculations (real calculations require calibration data)
            BME280Data bmeData;
            bmeData.temperature = (float)adc_t / 16.0f;
            bmeData.pressure = (float)adc_p / 100.0f;
            bmeData.humidity = (float)adc_h / 1024.0f;

            return bmeData;
        }
    }
    
    printf("Failed to read BME280!\n");
    return {0, 0, 0};  // Return default values in case of failure
}

