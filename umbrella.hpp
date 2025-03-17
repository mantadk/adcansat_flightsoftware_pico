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

bool isUpsideDown();
// void initBME280();
// BME280Data readBME280();
// int sendString(const std::string& data);
void sendVUARTString(std::string data);
bool bme280_init(i2c_inst_t *i2c);
std::string bme280_read_measurements_string(i2c_inst_t *i2c, struct bme280_calib_data *calib);