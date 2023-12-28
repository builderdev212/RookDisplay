#ifndef ESP32_S3_SPI_Driver_h
#define ESP32_S3_SPI_Driver_h

// Processor specific SPI libraries
#include "soc/spi_reg.h"
#include "driver/spi_master.h"

#define SPI_PORT HSPI

#define REG_SPI_BASE(i) DR_REG_SPI3_BASE
#define _spi_cmd       (uint32_t*)(SPI_CMD_REG(SPI_PORT))
#define _spi_user      (uint32_t*)(SPI_USER_REG(SPI_PORT))
#define _spi_mosi_dlen (uint32_t*)(SPI_MS_DLEN_REG(SPI_PORT))
#define _spi_w         (uint32_t*)(SPI_W0_REG(SPI_PORT))

#define SET_BUS_WRITE_MODE *_spi_user = SPI_USR_MOSI
#define SET_BUS_READ_MODE  *_spi_user = SPI_USR_MOSI | SPI_USR_MISO | SPI_DOUTDIN

#define SPI_BUSY_CHECK while (*_spi_cmd&SPI_USR)

// TFT_DC data/command pin
#define DC_C GPIO.out_w1tc = (1 << DISPLAY_DC)
#define DC_D GPIO.out_w1ts = (1 << DISPLAY_DC)

// TFT_CS chip select pin
#define CS_L GPIO.out_w1tc = (1 << DISPLAY_CS); GPIO.out_w1tc = (1 << DISPLAY_CS)
#define CS_H GPIO.out_w1ts = (1 << DISPLAY_CS)

// SPI Write

// Write 8 bits to TFT
#define displayWrite8(bits)   spi.transfer(bits)

// Convert 16 bit colour to 18 bit and write in 3 bytes
#define displayWrite16(C)  spi.transfer(((C) & 0xF800)>>8); \
                           spi.transfer(((C) & 0x07E0)>>3); \
                           spi.transfer(((C) & 0x001F)<<3)


// Convert swapped byte 16 bit colour to 18 bit and write in 3 bytes
#define displayWrite16S(C) spi.transfer((C) & 0xF8); \
                        spi.transfer(((C) & 0xE000)>>11 | ((C) & 0x07)<<5); \
                        spi.transfer(((C) & 0x1F00)>>5)

// Write 32 bits to TFT
#define tft_Write_32(C)  spi.write32(C)

// Write two concatenated 16 bit values to TFT
#define displayWrite32C(C,D) spi.write32((C)<<16 | (D))

// Write 16 bit value twice to TFT
#define displayWrite32D(C)  spi.write32((C)<<16 | (C))

// SPI Read

#define tft_Read_8() spi.transfer(0)

// why is this
#define DAT8TO32(P) ( (uint32_t)P[0]<<8 | P[1] | P[2]<<24 | P[3]<<16 )

#endif