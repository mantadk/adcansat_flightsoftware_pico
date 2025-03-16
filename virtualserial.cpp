#include "umbrella.hpp"

void waitForReceive() {
    //write high on virtual notify line
    gpio_put(VIRTUAL_NOTIFY_LINE, 1);
    //write low on clock
    gpio_put(VIRTUAL_CLOCK_LINE, 0);
    //wait for enable line to go high
    while (gpio_get(VIRTUAL_ENABLE_LINE) == 0) {
    }
}

void sendData(uint8_t data) {
    //convert data to true - false array
    bool bits[8];
    for (int i = 0; i < 8; i++) {
        bits[i] = (data >> i) & 1;
    }
    //write data to data line, while clocking for each bit
    for (int i = 0; i < 8; i++) {
        gpio_put(VIRTUAL_DATA_LINE, bits[i]);
        gpio_put(VIRTUAL_CLOCK_LINE, 1);
        sleep_ms(10);
        gpio_put(VIRTUAL_CLOCK_LINE, 0);
        sleep_ms(10);
    }
}

int sendString(std::string str) {
    //send each character in the string
    waitForReceive();
    for (int i = 0; i < str.length(); i++) {
        uint8_t data = str[i];
        sendData(data);
    }
    //send newline character
    uint8_t data = '\n';
    sendData(data);
    return 0;
}