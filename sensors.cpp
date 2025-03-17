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

const uint8_t BME_REG_CALIB_dig_T1 = 0x88; //7:0, 15:8, unsigned short
const uint8_t BME_REG_CALIB_dig_T2 = 0x8A; //7:0, 15:8, signed short
const uint8_t BME_REG_CALIB_dig_T3 = 0x8C; //7:0, 15:8, signed short
const uint8_t BME_REG_CALIB_dig_P1 = 0x8E; //7:0, 15:8, unsigned short
const uint8_t BME_REG_CALIB_dig_P2 = 0x90; //7:0, 15:8, signed short
const uint8_t BME_REG_CALIB_dig_P3 = 0x92; //7:0, 15:8, signed short
const uint8_t BME_REG_CALIB_dig_P4 = 0x94; //7:0, 15:8, signed short
const uint8_t BME_REG_CALIB_dig_P5 = 0x96; //7:0, 15:8, signed short
const uint8_t BME_REG_CALIB_dig_P6 = 0x98; //7:0, 15:8, signed short
const uint8_t BME_REG_CALIB_dig_P7 = 0x9A; //7:0, 15:8, signed short
const uint8_t BME_REG_CALIB_dig_P8 = 0x9C; //7:0, 15:8, signed short
const uint8_t BME_REG_CALIB_dig_P9 = 0x9E; //7:0, 15:8, signed short
const uint8_t BME_REG_CALIB_dig_H1 = 0xA1; //7:0, unsigned char
const uint8_t BME_REG_CALIB_dig_H2 = 0xE1; //7:0, 15:8, signed short
const uint8_t BME_REG_CALIB_dig_H3 = 0xE3; //7:0, unsigned char
const uint8_t BME_REG_CALIB_dig_H4 = 0xE4; //7:0, 15:8, signed short
const uint8_t BME_REG_CALIB_dig_H5 = 0xE5; //7:0, 15:8, signed short

/*
Both pressure and temperature values are expected to be received in 
    20 bit format, 
    positive, 
    stored in a 32 bit signed integer. 
Humidity is expected to be received in 
    16 bit format, 
    positive,
    stored in a 32 bit signed integer.
*/


void init_bme280(i2c_inst_t *i2c)
{
    //clock: 100kHz
    //sensor mode: forced mode
    //highest oversampling for temperature, pressure and humidity
    //highest filter coefficient
    //highest resolution
    uint8_t sensor_mode = 0b01; //forced mode
    uint8_t temp_oversampling = 0b101; //x16
    uint8_t press_oversampling = 0b101; //x16
    uint8_t hum_oversampling = 0b101; //x16
    uint8_t filter_coefficient = 0b100; //16
    uint8_t spi3w_en = 0b0; //3-wire SPI disabled
    uint8_t config = (spi3w_en << 0) | (filter_coefficient << 2);
    uint8_t ctrl_meas = (temp_oversampling << 5) | (press_oversampling << 2) | sensor_mode;
    uint8_t ctrl_hum = hum_oversampling;
    bme280_write_byte(i2c, BME280_REG_CTRL_MEAS, ctrl_meas);
    bme280_write_byte(i2c, BME280_REG_CONFIG, config);
    bme280_write_byte(i2c, BME280_REG_CTRL_MEAS, ctrl_meas);
}


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


bme280_calib_data_temp Read_Temperature_Calibration_Data(i2c_inst_t *i2c)
{
    bme280_calib_data_temp calib;
    uint8_t data[6];
    bme280_read_bytes(i2c, BME_REG_CALIB_dig_T1, data, 2);
    calib.dig_T1 = (data[1] << 8) | data[0];
    bme280_read_bytes(i2c, BME_REG_CALIB_dig_T2, data, 2);
    calib.dig_T2 = (data[1] << 8) | data[0];
    bme280_read_bytes(i2c, BME_REG_CALIB_dig_T3, data, 2);
    calib.dig_T3 = (data[1] << 8) | data[0];
    return calib;
}

long signed int Read_Temperature(i2c_inst_t *i2c)
{
    uint8_t data[3];
    bme280_read_bytes(i2c, BME280_REG_TEMP_MSB, data, 3);
    long signed int raw = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
    return raw;
}

long signed int Compensate_Temperature(long signed int raw, bme280_calib_data_temp calib)
{
    unsigned short dig_T1, signed short dig_T2, signed short dig_T3;
    dig_T1 = calib.dig_T1;
    dig_T2 = calib.dig_T2;
    dig_T3 = calib.dig_T3;
    long signed int var1, var2, T;
    var1 = ((((raw>>3)-((long signed int)dig_T1<<1)))*((long signed int)dig_T2))>>11;
    var2 = (((((raw>>4)-((long signed int)dig_T1))*((raw>>4)-((long signed int)dig_T1)))>>12)*((long signed int)dig_T3))>>14;
    T = ((var1 + var2) * 5 + 128) >> 8;
    return T;
}