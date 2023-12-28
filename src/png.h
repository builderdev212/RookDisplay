#ifndef PNG_DECODE_H
#define PNG_DECODE_H

#include <Arduino.h>
#include <FS.h>

#include "SupportLibs/png_data_types.h"

#define PNG_DEBUG

class png_decode {
    public:
        
        bool decode(fs::FS &fs, const char *filename);
    private:
        // FILE READING FUNCTIONS //

        // Read PNG from a file and return it as a rawPNGFile.
        rawPNGFile readFile(fs::FS &fs, const char *filename);

        // CHUNK PARSING FUNCTIONS //

        // Read chunks from file.
        PNGChunks readChunks(const rawPNGFile png);

        // Read chunk of data from buffer into smaller array.
        PNGChunk readChunk(const uint8_t *data);

        // Parse the PNG header from the raw data.
        PNGHeader readHeader(const PNGChunk header);

        // CHUNK CHECKING FUNCTIONS //

        // Check the file signature.
        bool checkSignature(const uint8_t *data);

        // HELPER FUNCTIONS //

        // Get number of bits per pixel.
        uint8_t calcBpp(const uint8_t colorType, const uint8_t bitDepth);

        // Check if chunk is IEND Chunk.
        bool isEndChunk(const PNGChunk endChunk);

        // Read 32b int from 8b array
        inline size_t read32BitInt(const uint8_t *data);
};

#endif