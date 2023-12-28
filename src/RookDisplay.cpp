#include "RookDisplay.h"
#include "Processors/ESP32_S3_SPI_Driver.cpp"
#include "Displays/ILI9488.cpp"


// Init

RookDisplay::RookDisplay(uint16_t displayWidth, uint16_t displayHeight, uint8_t displayRotation, bool displayInversion) {
    _displayWidth = int(displayWidth);
    _displayHeight = int(displayHeight);
    _displayRotation = int(displayRotation);
    _inverted = bool(displayInversion);

    _booted = true;
}

void RookDisplay::init() {
    if (_booted) {
        initSPIBus();

        #ifdef DISPLAY_MOSI
            spi.begin(DISPLAY_SCLK, DISPLAY_MOSI, DISPLAY_MOSI, -1);
        #else
            spi.begin();
        #endif

        _booted = false;
        _endDisplayWrite();
    }

    // Reset the display.
    #ifdef DISPLAY_RST
        _writeCommand(CMD_NOP);
        digitalWrite(DISPLAY_RST, HIGH);
        delay(5);
        digitalWrite(DISPLAY_RST, LOW);
        delay(20);
        digitalWrite(DISPLAY_RST, HIGH);
    #endif

    delay(150);

    _beginDisplayWrite();
    // Defined based on display driver
    _initDisplay();

    if (_inverted) {
        _writeCommand(CMD_INVERT_ON);
    } else {
        _writeCommand(CMD_INVERT_OFF);
    }

    _endDisplayWrite();

    // Set display to default rotation
    setRotation(_displayRotation);
}

void RookDisplay::setRotation(uint8_t newRotation) {
    _displayRotation = newRotation % 4;
    _beginDisplayWrite();
    _rotateDisplay();
    delayMicroseconds(10);
    _endDisplayWrite();   
}

void RookDisplay::setWindow(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1) {
    SPI_BUSY_CHECK;
    DC_C; 
    displayWrite8(CMD_XADDR_SET);
    DC_D;
    displayWrite32C(x0, x1);
    DC_C;
    displayWrite8(CMD_YADDR_SET);
    DC_D;
    displayWrite32C(y0, y1);
    DC_C;
    displayWrite8(CMD_MEM_WRITE);
    DC_D;
}

void RookDisplay::initSPIBus() {
    #ifdef DISPLAY_CS
        pinMode(DISPLAY_CS, OUTPUT);
        // Set chip select to inactive.
        digitalWrite(DISPLAY_CS, HIGH);
    #endif

    #ifdef DISPLAY_DC
        pinMode(DISPLAY_DC, OUTPUT);
        // Set data/command to data mode.
        digitalWrite(DISPLAY_DC, HIGH);
    #endif

    #ifdef DISPLAY_RST
        pinMode(DISPLAY_RST, OUTPUT);
        // Set reset to high.
        digitalWrite(DISPLAY_RST, HIGH);
    #endif
}

// Drawing functions
void RookDisplay::drawPixel(int32_t x, int32_t y, uint32_t color) {
    _beginDisplayWrite();
    SPI_BUSY_CHECK;

    if (x_addr != x) {
        DC_C;
        displayWrite8(CMD_XADDR_SET);
        DC_D;
        displayWrite32D(x);
        x_addr = x;
    }

    if (y_addr != y) {
        DC_C;
        displayWrite8(CMD_YADDR_SET);
        DC_D;
        displayWrite32D(y);
        y_addr = y;
    }

    DC_C;
    displayWrite8(CMD_MEM_WRITE);
    DC_D;
    displayWrite16(color);

    _endDisplayWrite();
}

void RookDisplay::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
    _beginDisplayWrite();
    setWindow(x, y, x+w-1, y+h-1);
    pushBlock(color, w*h);
    _endDisplayWrite();
}

// Display read/write functions

inline void RookDisplay::_beginDisplayWrite() {
    if (_locked) {
        _locked = false;
        spi.beginTransaction(SPISettings(SPI_FREQUENCY, MSBFIRST, SPI_MODE0));
        CS_L;
        SET_BUS_WRITE_MODE;
    }
}

inline void RookDisplay::_endDisplayWrite() {
    if (!_busy) {
        if (!_locked) {
            _locked = true;
            SPI_BUSY_CHECK;
            CS_H;
            SET_BUS_READ_MODE;
            spi.endTransaction();
        }
    }
}

void RookDisplay::_writeCommand(uint8_t command) {
    _beginDisplayWrite();
    // Set the input to command mode.
    DC_C;
    // Write the command.
    displayWrite8(command);
    // Set the input back to data mode.
    DC_D;
    _endDisplayWrite();
}

void RookDisplay::_writeData(uint8_t data) {
    _beginDisplayWrite();
    // Set the input to data mode.
    DC_D;
    // Write the data.
    displayWrite8(data);
    // Allows more hold time for low VDI rail... will look at this later again.
    CS_L;
    _endDisplayWrite();
}