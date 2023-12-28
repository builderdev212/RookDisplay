#include <Arduino.h>
#include <RookDisplay.h>


RookDisplay *display = new RookDisplay();


void setup() {
    display->init();
    display->fillRect(0, 0, 320, 480, 0x001F);
    display->drawPixel(100, 100, 0x001F);
    display->drawPixel(100, 101, 0x001F);
    display->drawPixel(101, 101, 0x001F);
    display->drawPixel(101, 100, 0x001F);
}

void loop() {
}
