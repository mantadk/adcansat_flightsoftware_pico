#include "pico/stdlib.h"
#include "hardware/i2c.h"     // I2C hardware functions
#include "hardware/gpio.h"    // GPIO hardware functions
#include <string>

#define LED 25
#define UPSIDE_DOWN_SENSOR 10

#define VIRTUAL_DATA_LINE 2
#define VIRTUAL_CLOCK_LINE 3
#define VIRTUAL_ENABLE_LINE 4
#define VIRTUAL_NOTIFY_LINE 5

typedef struct BME280Data {
    float temperature;
    float pressure;
    float humidity;
} BME280Data;

struct bme280_calib_data {
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;
    uint8_t dig_H1;
    int16_t dig_H2;
    uint8_t dig_H3;
    int16_t dig_H4;
    int16_t dig_H5;
    int8_t dig_H6;
};

bool isUpsideDown();
// void initBME280();
// BME280Data readBME280();
// int sendString(const std::string& data);
void sendVUARTString(std::string data);
bool bme280_init(i2c_inst_t *i2c);
void bme280_read_calibration_data(i2c_inst_t *i2c, struct bme280_calib_data *calib);
std::string bme280_read_measurements_string(i2c_inst_t *i2c, struct bme280_calib_data *calib);