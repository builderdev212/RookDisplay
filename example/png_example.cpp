#include <Arduino.h>
#include <png.h>
#include <SD.h>
#include <SPI.h>

png_decode pngTest;

void logMemory() {
  log_d("Used PSRAM: %d", ESP.getPsramSize() - ESP.getFreePsram());
}

void setup() {
    // initiate SD card
    if (!SD.begin(false)) {
        log_w("Unable to initiate SD card.");
        return;
    }

    logMemory();
    pngTest.decode(SD, "/thing_blue.png");
    logMemory();
}

void loop() {
}
