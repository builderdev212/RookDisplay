#ifndef PNG_DATA_TYPES
#define PNG_DATA_TYPES

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
    uint8_t *data = nullptr;
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

#endif