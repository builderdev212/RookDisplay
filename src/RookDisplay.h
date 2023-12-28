#ifndef RookDisplay_h
#define RookDisplay_h

#define ROOK_DISPLAY_VERSION "0.1"
#define ROOK_DISPLAY_ERROR

// Neccessary libraries
#include <Arduino.h>
#include <Print.h>
#include <SPI.h>

// Pin defintions
#include "Displays/RookDisplayPins.h"

// Display Specific definitions
#include "Displays/ILI9488.h"

// Processor SPI driver
#include "Processors/ESP32_S3_SPI_Driver.h"

class RookDisplay {
    public:
        // library init
        RookDisplay(uint16_t displayWidth = DISPLAY_WIDTH, uint16_t displayHeight = DISPLAY_HEIGHT, uint8_t displayRotation = DISPLAY_ROTATION, bool displayInversion = DISPLAY_INVERSION);

        // initiate the display
        void init();

        // display related functions
        void setRotation(uint8_t newRotation);
        void setWindow(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1);

        // drawing functions
        void drawPixel(int32_t x, int32_t y, uint32_t color);
        void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);

    private:
        // display write functions
        inline void _beginDisplayWrite() __attribute__((always_inline));
        inline void _endDisplayWrite() __attribute__((always_inline));

        // Initiate the spi data bus
        void initSPIBus();
        
        // Display specific functions
        void _initDisplay();
        void _rotateDisplay();


        // Send 8 bit command to display
        void _writeCommand(uint8_t command);
        void _writeData(uint8_t data);

        // Display variables
        uint16_t _displayWidth, _displayHeight;
        uint8_t _displayRotation;
        bool _inverted;

        // Display state functions
        bool _booted;
        bool _busy;
        bool _locked;

        // Draw state functions
        uint32_t x_addr = 0;
        uint32_t y_addr = 0;

        // drawing pixels
        void pushBlock(uint16_t color, uint32_t len);
};

#endif