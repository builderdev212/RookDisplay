#ifndef PNG_DECODE_H
#define PNG_DECODE_H

#include <Arduino.h>
#include <FS.h>

#define MINIZ_NO_STDIO
#define MINIZ_NO_ARCHIVE_APIS
#define MINIZ_NO_TIME
#define MINIZ_NO_ZLIB_APIS
#define MINIZ_NO_MALLOC
#include <miniz.h>

#define INPUT_BUFFER_SIZE (32*1024)
#define OUTPUT_BUFFER_SIZE (32*1024)

#define PNG_DEBUG

// Struct to store raw data read in from file.
struct rawPNGFile {
    size_t size = 0;
    uint8_t *data = nullptr;
    bool valid = true;
};

// Struct to store individual chunk data.
struct PNGChunk {
    uint32_t size = 0;
    char *name = nullptr;
    unsigned char *data = nullptr;
    uint32_t crc = 0;
    bool valid = true;
};

// Struct to store array of chunks.
struct PNGChunks{
    uint8_t num = 0;
    PNGChunk *chunks = nullptr;
    bool valid = true;
};

// Struct to store header info.
struct PNGHeader {
    uint32_t width = 0;
    uint32_t height = 0;
    uint8_t bitDepth = 0;
    uint8_t colorType = 0;
    uint8_t filterType = 0;
    bool interlaceAdam7 = false;
    bool valid = true;
};

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