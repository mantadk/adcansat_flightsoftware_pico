#include "umbrella.hpp"

void waitForReceive() {
    //write high on virtual notify line
    gpio_put(VIRTUAL_NOTIFY_LINE, 1);
    //write low on clock
    gpio_put(VIRTUAL_CLOCK_LINE, 0);
    //write low on data
    gpio_put(VIRTUAL_DATA_LINE, 0);
    //wait for enable line to go high
    gpio_put(LED, 1);
    while (gpio_get(VIRTUAL_ENABLE_LINE) == 0) {
        //blink led 
        gpio_put(LED, 0);
        sleep_ms(50);
        gpio_put(LED, 1);
        sleep_ms(50);
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
        sleep_ms(100);
        gpio_put(VIRTUAL_CLOCK_LINE, 0);
        sleep_ms(100);
    }
}

int sendString(const std::string& data) {
    // Send each character in the string
    for (size_t i = 0; i < data.length(); i++) {
        uint8_t ch = static_cast<uint8_t>(data[i]);
        waitForReceive();
        sendData(ch);
    }

    // Send newline character
    uint8_t endChar = '\n';
    waitForReceive();
    sendData(endChar);
    gpio_put(VIRTUAL_NOTIFY_LINE, 0);
    gpio_put(LED, 0);
    return 0;
}
