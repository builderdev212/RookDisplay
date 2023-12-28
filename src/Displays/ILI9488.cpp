#ifndef ILI9488_INIT_H
#define ILI9488_INIT_H

#include "../RookDisplay.h"

void RookDisplay::_initDisplay() {
    // Positive Gamma Control
    _writeCommand(0xE0);
    _writeData(0x00);
    _writeData(0x03);
    _writeData(0x09);
    _writeData(0x08);
    _writeData(0x16);
    _writeData(0x0A);
    _writeData(0x3F);
    _writeData(0x78);
    _writeData(0x4C);
    _writeData(0x09);
    _writeData(0x0A);
    _writeData(0x08);
    _writeData(0x16);
    _writeData(0x1A);
    _writeData(0x0F);

    // Negative Gamma Control
    _writeCommand(0XE1);
    _writeData(0x00);
    _writeData(0x16);
    _writeData(0x19);
    _writeData(0x03);
    _writeData(0x0F);
    _writeData(0x05);
    _writeData(0x32);
    _writeData(0x45);
    _writeData(0x46);
    _writeData(0x04);
    _writeData(0x0E);
    _writeData(0x0D);
    _writeData(0x35);
    _writeData(0x37);
    _writeData(0x0F);

    // Power Control 1
    _writeCommand(0XC0);
    _writeData(0x17);
    _writeData(0x15);

    // Power Control 2
    _writeCommand(0xC1);
    _writeData(0x41);

    // VCOM Control
    _writeCommand(0xC5);
    _writeData(0x00);
    _writeData(0x12);
    _writeData(0x80);

    // Memory Access Control
    _writeCommand(CMD_MEM_ACCESS_CTRL);
    _writeData(0x48); // said MX and BGR... look into this later

    // Pixel Interface Format
    _writeCommand(0x3A);
    _writeData(0x66);

    // Interface Mode Control
    _writeCommand(0xB0);
    _writeData(0x00);

    // Frame Rate Control
    _writeCommand(0xB1);
    _writeData(0xA0);

    // Display Inversion Control
    _writeCommand(0xB4);
    _writeData(0x02);

    // Display Function Control
    _writeCommand(0xB6);
    _writeData(0x02);
    _writeData(0x02);
    _writeData(0x3B);

    // Entry Mode Set
    _writeCommand(0xB7);
    _writeData(0xC6);

    // Adjust Control 3
    _writeCommand(0xF7);
    _writeData(0xA9);
    _writeData(0x51);
    _writeData(0x2C);
    _writeData(0x82);

    // Exit Sleep
    _writeCommand(CMD_EXIT_SLEEP);
    delay(120);

    // Turn Display On
    _writeCommand(CMD_DISPLAY_ON);
    delay(25);
}

void RookDisplay::_rotateDisplay() {
    _writeCommand(CMD_MEM_ACCESS_CTRL);
    
    switch (_displayRotation) {
        // Landscape
        case 0:
            _writeData(CMD_MEM_ACCESS_MX | CMD_MEM_ACCESS_BGR);
            _displayWidth = DISPLAY_WIDTH;
            _displayHeight = DISPLAY_HEIGHT;
            break;
        // Portrait
        case 1:
            _writeData(CMD_MEM_ACCESS_MV | CMD_MEM_ACCESS_BGR);
            _displayWidth = DISPLAY_HEIGHT;
            _displayHeight = DISPLAY_WIDTH;
            break;
        // Inverted Landscape
        case 2:
            _writeData(CMD_MEM_ACCESS_MY | CMD_MEM_ACCESS_BGR);
            _displayWidth = DISPLAY_WIDTH;
            _displayHeight = DISPLAY_HEIGHT;
            break;
        // Inverted Portrait
        case 3:
            _writeData(CMD_MEM_ACCESS_MX | CMD_MEM_ACCESS_MY | CMD_MEM_ACCESS_MV | CMD_MEM_ACCESS_BGR);
            _displayWidth = DISPLAY_HEIGHT;
            _displayHeight = DISPLAY_WIDTH;
            break;
        default:
            #ifdef ROOK_DISPLAY_ERROR
                Serial.println("Error: _displayRotation was expected to be a value between 0 and 3, got "+String(_displayRotation)+" instead.");
            #endif
            break;
    }
}

#endif
