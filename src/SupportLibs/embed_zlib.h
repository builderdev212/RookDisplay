#ifndef EMBED_ZLIB_H
#define EMBED_ZLIB_H

#include <Arduino.h>

class embed_zlib {
    public:
        uint8_t * decode(const PNGChunk data);
    private:

}

#endif