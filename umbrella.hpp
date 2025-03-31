#include "pico/stdlib.h"
#include "hardware/i2c.h"     // I2C hardware functions
#include "hardware/gpio.h"    // GPIO hardware functions
#include <string>
#include <iomanip>
#include <sstream>

#define LED 25

#define VIRTUAL_DATA_LINE 2
#define VIRTUAL_CLOCK_LINE 3
#define VIRTUAL_ENABLE_LINE 4
#define VIRTUAL_NOTIFY_LINE 5

typedef struct BME280Data {
    float temperature;
    float pressure;
    float humidity;
} BME280Data;

struct bme280_calib_data_temp {
    unsigned short dig_T1;
    signed short dig_T2;
    signed short dig_T3;
};

struct bme280_calib_data_press {
    unsigned short dig_P1;
    signed short dig_P2;
    signed short dig_P3;
    signed short dig_P4;
    signed short dig_P5;
    signed short dig_P6;
    signed short dig_P7;
    signed short dig_P8;
    signed short dig_P9;
};

struct bme280_calib_data_hum {
    unsigned char dig_H1;
    signed short dig_H2;
    unsigned char dig_H3;
    signed short dig_H4;
    signed short dig_H5;
};

void sendVUARTString(std::string data);

//BME280 functions
void init_bme280(i2c_inst_t *i2c);
//temperature
bme280_calib_data_temp Read_Temperature_Calibration_Data(i2c_inst_t *i2c);
long signed int Read_Temperature(i2c_inst_t *i2c);
long signed int Compensate_Temperature(long signed int raw, bme280_calib_data_temp calib);
//pressure