#include "umbrella.hpp"

void waitForReceive() {
    //write low on clock
    gpio_put(VIRTUAL_CLOCK_LINE, 0);
    //write low on data
    gpio_put(VIRTUAL_DATA_LINE, 0);
    //Wait until master tells me to send data #arkeugbksuyrehg
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
    //write low on clock
    gpio_put(VIRTUAL_CLOCK_LINE, 0);
    //write data to data line, while clocking for each bit
    for (int i = 0; i < 8; i++) {
        gpio_put(VIRTUAL_DATA_LINE, bits[i]); //#seruibgiuserh
        sleep_ms(100);
        gpio_put(VIRTUAL_CLOCK_LINE, 1); //#srleiuhsdkufgb
        sleep_ms(50);
        gpio_put(VIRTUAL_CLOCK_LINE, 0); //#lsiuerhlugvs
        sleep_ms(100);
    }
}

int sendString(const std::string& data) {
    //Notify master, that there is data to be sent #srieughvbe
    gpio_put(VIRTUAL_NOTIFY_LINE, 1);
    // Send each character in the string
    for (size_t i = 0; i < data.length(); i++) {
        uint8_t ch = static_cast<uint8_t>(data[i]);
        waitForReceive();
        sendData(ch);
    }
    gpio_put(VIRTUAL_NOTIFY_LINE, 0);
    gpio_put(LED, 0);
    return 0;
}
