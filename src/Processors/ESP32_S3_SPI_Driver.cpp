#include "../RookDisplay.h"

SPIClass spi = SPIClass(HSPI);

// Write a block of pixels to be the same color
void RookDisplay::pushBlock(uint16_t color, uint32_t len) {
    // Split out the colours
  uint32_t r = (color & 0xF800)>>8;
  uint32_t g = (color & 0x07E0)<<5;
  uint32_t b = (color & 0x001F)<<19;
  // Concatenate 4 pixels into three 32 bit blocks
  uint32_t r0 = r<<24 | b | g | r;
  uint32_t r1 = r0>>8 | g<<16;
  uint32_t r2 = r1>>8 | b<<8;

  if (len > 19)
  {
    WRITE_PERI_REG(SPI_MS_DLEN_REG(SPI_PORT), 479);

    while(len>19)
    {
      while (READ_PERI_REG(SPI_CMD_REG(SPI_PORT))&SPI_USR);
      WRITE_PERI_REG(SPI_W0_REG(SPI_PORT), r0);
      WRITE_PERI_REG(SPI_W1_REG(SPI_PORT), r1);
      WRITE_PERI_REG(SPI_W2_REG(SPI_PORT), r2);
      WRITE_PERI_REG(SPI_W3_REG(SPI_PORT), r0);
      WRITE_PERI_REG(SPI_W4_REG(SPI_PORT), r1);
      WRITE_PERI_REG(SPI_W5_REG(SPI_PORT), r2);
      WRITE_PERI_REG(SPI_W6_REG(SPI_PORT), r0);
      WRITE_PERI_REG(SPI_W7_REG(SPI_PORT), r1);
      WRITE_PERI_REG(SPI_W8_REG(SPI_PORT), r2);
      WRITE_PERI_REG(SPI_W9_REG(SPI_PORT), r0);
      WRITE_PERI_REG(SPI_W10_REG(SPI_PORT), r1);
      WRITE_PERI_REG(SPI_W11_REG(SPI_PORT), r2);
      WRITE_PERI_REG(SPI_W12_REG(SPI_PORT), r0);
      WRITE_PERI_REG(SPI_W13_REG(SPI_PORT), r1);
      WRITE_PERI_REG(SPI_W14_REG(SPI_PORT), r2);
#if CONFIG_IDF_TARGET_ESP32S3
      SET_PERI_REG_MASK(SPI_CMD_REG(SPI_PORT), SPI_UPDATE);
      while (READ_PERI_REG(SPI_CMD_REG(SPI_PORT))&SPI_UPDATE);
#endif
      SET_PERI_REG_MASK(SPI_CMD_REG(SPI_PORT), SPI_USR);
      len -= 20;
    }
    while (READ_PERI_REG(SPI_CMD_REG(SPI_PORT))&SPI_USR);
  }

  if (len)
  {
    WRITE_PERI_REG(SPI_MS_DLEN_REG(SPI_PORT), (len * 24) - 1);
    WRITE_PERI_REG(SPI_W0_REG(SPI_PORT), r0);
    WRITE_PERI_REG(SPI_W1_REG(SPI_PORT), r1);
    WRITE_PERI_REG(SPI_W2_REG(SPI_PORT), r2);
    WRITE_PERI_REG(SPI_W3_REG(SPI_PORT), r0);
    WRITE_PERI_REG(SPI_W4_REG(SPI_PORT), r1);
    WRITE_PERI_REG(SPI_W5_REG(SPI_PORT), r2);
    if (len > 8 )
    {
      WRITE_PERI_REG(SPI_W6_REG(SPI_PORT), r0);
      WRITE_PERI_REG(SPI_W7_REG(SPI_PORT), r1);
      WRITE_PERI_REG(SPI_W8_REG(SPI_PORT), r2);
      WRITE_PERI_REG(SPI_W9_REG(SPI_PORT), r0);
      WRITE_PERI_REG(SPI_W10_REG(SPI_PORT), r1);
      WRITE_PERI_REG(SPI_W11_REG(SPI_PORT), r2);
      WRITE_PERI_REG(SPI_W12_REG(SPI_PORT), r0);
      WRITE_PERI_REG(SPI_W13_REG(SPI_PORT), r1);
      WRITE_PERI_REG(SPI_W14_REG(SPI_PORT), r2);
    }
#if CONFIG_IDF_TARGET_ESP32S3
    SET_PERI_REG_MASK(SPI_CMD_REG(SPI_PORT), SPI_UPDATE);
    while (READ_PERI_REG(SPI_CMD_REG(SPI_PORT))&SPI_UPDATE);
#endif
    SET_PERI_REG_MASK(SPI_CMD_REG(SPI_PORT), SPI_USR);
    while (READ_PERI_REG(SPI_CMD_REG(SPI_PORT))&SPI_USR);
  }
}


/***************************************************************************************
** Function name:           pushPixels - for ESP32 and 3 byte RGB display
** Description:             Write a sequence of pixels
***************************************************************************************/

/*
void RookDisplay::pushPixels(const void* data_in, uint32_t len){

  uint16_t *data = (uint16_t*)data_in;
  // ILI9488 write macro is not endianess dependant, hence !_swapBytes
  if(!_swapBytes) { while ( len-- ) {displayWrite16S(*data); data++;} }
  else { while ( len-- ) {displayWrite16(*data); data++;} }
}

/***************************************************************************************
** Function name:           pushSwapBytePixels - for ESP32 and 3 byte RGB display
** Description:             Write a sequence of pixels with swapped bytes
***************************************************************************************/

/*
void TFT_eSPI::pushSwapBytePixels(const void* data_in, uint32_t len){

  uint16_t *data = (uint16_t*)data_in;
  // ILI9488 write macro is not endianess dependant, so swap byte macro not used here
  while ( len-- ) {tft_Write_16(*data); data++;}
}

*/