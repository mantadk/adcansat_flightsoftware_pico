#include "umbrella.hpp"

bool isUpsideDown()
{                                           // Is the sensor upside down?
    return !(gpio_get(UPSIDE_DOWN_SENSOR)); // Read and return the state
}

// read BME 280 sensor data via I2C

// void initBME280() {
//     i2c_init(i2c0, 100000);
//     gpio_set_function(0, GPIO_FUNC_I2C);
//     gpio_set_function(1, GPIO_FUNC_I2C);
//     gpio_pull_up(0);
//     gpio_pull_up(1);
//     uint8_t reset_cmd[2] = {0xE0, 0xB6};
//     i2c_write_blocking(i2c0, 0x76, reset_cmd, 2, false);
//     sleep_ms(10);
// }

// BME280 Registers
const uint8_t BME280_ADDR = 0x76; // Or 0x77, check your sensor
const uint8_t BME280_REG_ID = 0xD0;
const uint8_t BME280_REG_CTRL_MEAS = 0xF4;
const uint8_t BME280_REG_CONFIG = 0xF5;
const uint8_t BME280_REG_PRESS_MSB = 0xF7;
const uint8_t BME280_REG_TEMP_MSB = 0xFA;
const uint8_t BME280_REG_HUM_MSB = 0xFD;
const uint8_t BME280_REG_CALIB_00 = 0x88;
const uint8_t BME280_REG_CALIB_26 = 0xE1;

// Function to read a single byte from the BME280
uint8_t bme280_read_byte(i2c_inst_t *i2c, uint8_t reg)
{
    uint8_t data;
    i2c_write_blocking(i2c, BME280_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c, BME280_ADDR, &data, 1, false);
    return data;
}

// Function to read multiple bytes from the BME280
void bme280_read_bytes(i2c_inst_t *i2c, uint8_t reg, uint8_t *buf, uint8_t len)
{
    i2c_write_blocking(i2c, BME280_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c, BME280_ADDR, buf, len, false);
}

// Function to write a single byte to the BME280
void bme280_write_byte(i2c_inst_t *i2c, uint8_t reg, uint8_t data)
{
    uint8_t buf[] = {reg, data};
    i2c_write_blocking(i2c, BME280_ADDR, buf, 2, false);
}

// Function to read calibration data from the BME280
void bme280_read_calibration_data(i2c_inst_t *i2c, struct bme280_calib_data *calib)
{
    uint8_t calib_data[32];
    bme280_read_bytes(i2c, BME280_REG_CALIB_00, calib_data, 26);
    bme280_read_bytes(i2c, BME280_REG_CALIB_26, &calib_data[26], 7);

    calib->dig_T1 = calib_data[1] << 8 | calib_data[0];
    calib->dig_T2 = (int16_t)(calib_data[3] << 8 | calib_data[2]);
    calib->dig_T3 = (int16_t)(calib_data[5] << 8 | calib_data[4]);
    calib->dig_P1 = calib_data[7] << 8 | calib_data[6];
    calib->dig_P2 = (int16_t)(calib_data[9] << 8 | calib_data[8]);
    calib->dig_P3 = (int16_t)(calib_data[11] << 8 | calib_data[10]);
    calib->dig_P4 = (int16_t)(calib_data[13] << 8 | calib_data[12]);
    calib->dig_P5 = (int16_t)(calib_data[15] << 8 | calib_data[14]);
    calib->dig_P6 = (int16_t)(calib_data[17] << 8 | calib_data[16]);
    calib->dig_P7 = (int16_t)(calib_data[19] << 8 | calib_data[18]);
    calib->dig_P8 = (int16_t)(calib_data[21] << 8 | calib_data[20]);
    calib->dig_P9 = (int16_t)(calib_data[23] << 8 | calib_data[22]);
    calib->dig_H1 = calib_data[25];
    calib->dig_H2 = (int16_t)(calib_data[27] << 8 | calib_data[26]);
    calib->dig_H3 = calib_data[28];
    calib->dig_H4 = (int16_t)(calib_data[29] << 4 | (calib_data[30] & 0x0F));
    calib->dig_H5 = (int16_t)((calib_data[31] << 4) | (calib_data[30] >> 4));
    calib->dig_H6 = (int8_t)calib_data[32];
}

// Function to read temperature, pressure, and humidity from the BME280
void bme280_read_measurements(i2c_inst_t *i2c, struct bme280_calib_data *calib, float *temperature, float *pressure, float *humidity)
{
    uint8_t data[8];
    bme280_read_bytes(i2c, BME280_REG_PRESS_MSB, data, 8);

    int32_t raw_pressure = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
    int32_t raw_temperature = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);
    int32_t raw_humidity = (data[6] << 8) | data[7];

    // Temperature compensation
    int32_t var1 = ((((raw_temperature >> 3) - ((int32_t)calib->dig_T1 << 1))) * ((int32_t)calib->dig_T2)) >> 11;
    int32_t var2 = (((((raw_temperature >> 4) - ((int32_t)calib->dig_T1)) * ((raw_temperature >> 4) - ((int32_t)calib->dig_T1))) >> 12) * ((int32_t)calib->dig_T3)) >> 14;
    int32_t t_fine = var1 + var2;
    *temperature = (float)((t_fine * 5 + 128) >> 8) / 100.0f;

    // Pressure compensation
    var1 = ((int32_t)t_fine >> 1) - 64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t)calib->dig_P6);
    var2 = var2 + ((var1 * ((int32_t)calib->dig_P5)) << 1);
    var2 = (var2 >> 2) + (((int32_t)calib->dig_P4) << 16);
    var1 = (((int32_t)calib->dig_P3) * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3;
    var1 = var1 + ((((int32_t)calib->dig_P2) * var1) >> 1);
    var1 = var1 >> 18;
    var1 = (((32768 + var1)) * ((int32_t)calib->dig_P1)) >> 15;
    // Example of pressure calculation, review BME280 datasheet for correct calculation.
    if (var1 == 0)
    {
        *pressure = 0;
    }
    else
    {
        *pressure = ((1048576.0f - raw_pressure) - (var2 / 4096.0f)) * 3125.0f; // division instead of bitshift
        if (*pressure < 0x80000000)
        {
            *pressure = (*pressure * 2.0f) / var1; // multiplication and division instead of bitshift.
        }
        else
        {
            *pressure = (*pressure / var1) * 2.0f;
        }
        var1 = (((int32_t)calib->dig_P9) * ((int32_t)(*pressure / 8.0f)) * ((int32_t)(*pressure / 8.0f))) / 4096.0f; // division instead of bitshift.
        var2 = (((int32_t)(*pressure / 4.0f)) * ((int32_t)calib->dig_P8)) / 8192.0f;                                 // division instead of bitshift.
        *pressure = (float)*pressure + ((var1 + var2 + ((int32_t)calib->dig_P7)) / 16.0f);
        *pressure /= 100.0f; // Convert to hPa
    }

    // Humidity compensation
    int32_t v_x1_u32r;
    v_x1_u32r = (t_fine - ((int32_t)76800));
    v_x1_u32r = (((((raw_humidity << 14) - (((int32_t)calib->dig_H4) << 20) - (((int32_t)calib->dig_H5) * v_x1_u32r)) +
                   ((int32_t)16384)) >>
                  15) *
                 (((((((v_x1_u32r * ((int32_t)calib->dig_H6)) >> 10) *
                      (((v_x1_u32r * ((int32_t)calib->dig_H3)) >> 11) + ((int32_t)32768))) >>
                     10) +
                    ((int32_t)2097152)) *
                       ((int32_t)calib->dig_H2) +
                   8192) >>
                  14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                               ((int32_t)calib->dig_H1)) >>
                              4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    *humidity = (float)(v_x1_u32r >> 12) / 1024.0f;
}

bool bme280_init(i2c_inst_t *i2c)
{
    // Check device ID
    uint8_t chip_id = bme280_read_byte(i2c, BME280_REG_ID);
    if (chip_id != 0x60)
    {
        return false; // Device ID mismatch, initialization failed
    }

    // Set config register: t_sb = 0.5ms, filter = 4, spi3w_en = disabled
    bme280_write_byte(i2c, BME280_REG_CONFIG, 0b10010000);

    // Set ctrl_meas register: osrs_t = x1, osrs_p = x1, mode = normal
    bme280_write_byte(i2c, BME280_REG_CTRL_MEAS, 0b00100111); // normal mode

    return true; // Initialization successful
}

std::string bme280_read_measurements_string(i2c_inst_t *i2c, struct bme280_calib_data *calib)
{
    float temperature, pressure, humidity;
    bme280_read_measurements(i2c, calib, &temperature, &pressure, &humidity);

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2); // Set precision to 2 decimal places

    ss << "t:" << temperature << ";p:" << pressure << ";h:" << humidity;

    return ss.str();
}