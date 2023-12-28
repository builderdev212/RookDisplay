#ifndef ILI9488_DEFINES_H
#define ILI9488_DEFINES_H

#define DISPLAY_WIDTH     480
#define DISPLAY_HEIGHT    320
#define DISPLAY_INVERSION true
#define DISPLAY_ROTATION  0

#define CMD_NOP             0x00
#define CMD_SFT_RST         0x01

#define CMD_EXIT_SLEEP      0x11

#define CMD_INVERT_OFF      0x20
#define CMD_INVERT_ON       0x21

#define CMD_DISPLAY_ON      0x29

#define CMD_XADDR_SET   0x2A
#define CMD_YADDR_SET   0x2B
#define CMD_MEM_WRITE   0x2C

#define CMD_MEM_ACCESS_CTRL 0x36
#define CMD_MEM_ACCESS_MY   0x80
#define CMD_MEM_ACCESS_MX   0x40
#define CMD_MEM_ACCESS_MV   0x20
//#define CMD_MEM_ACCESS_ML   0x10
//#define CMD_MEM_ACCESS_RGB  0x00
#define CMD_MEM_ACCESS_BGR  0x08
//#define CMD_MEM_ACCESS_MH   0x04
//#define CMD_MEM_ACCESS_SS   0x02
//#define CMD_MEM_ACCESS_GS   0x01

#endif