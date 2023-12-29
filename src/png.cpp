#include "png.h"

bool png_decode::decode(fs::FS &fs, const char *filename) {
    rawPNGFile rawPNG;

    // read and store the png from the file.
    rawPNG = readFile(fs, filename);
    if (!rawPNG.valid) return false;

    // ensure file type was png.
    if (!checkSignature(rawPNG.data)) return false;

    // parse data into chunks to work with
    PNGChunks chunks = readChunks(rawPNG);
    if (!chunks.valid) return false;
    free(rawPNG.data);

    // ensure that last chunk is end chunk.
    if (!isEndChunk(chunks.chunks[chunks.num-1])) return false;

    // parse out header
    PNGHeader header = readHeader(chunks.chunks[0]);
    if (!header.valid) return false;

    uint8_t bpp = calcBpp(header.colorType, header.bitDepth);

/*   // Decompress the data.
    uint32_t original_size = ((header.width * (header.height * bpp + 7)) / 8) + header.height;

    tinfl_decompressor inflator;
    tinfl_init(&inflator);

    bool is_finished = false;

    uint infile_remaining = chunks.chunks[1].size;

    uint8_t *in_data_buffer = (uint8_t *)ps_calloc(INPUT_BUFFER_SIZE, sizeof(uint8_t));
    uint8_t *out_data_buffer = (uint8_t *)ps_calloc(original_size, sizeof(uint8_t));
    unsigned char *png_data = (unsigned char *)ps_calloc(((header.width * (header.height * bpp + 7)) / 8) + header.height, sizeof(unsigned char));

    const void *next_in = in_data_buffer;
    size_t avail_in = 0;
    void *next_out = out_data_buffer;
    size_t avail_out = original_size;
    size_t total_in = 0, total_out = 0;

    size_t in_bytes, out_bytes;
    tinfl_status status;

    while (!is_finished) {
        // Input buffer is empty, so read more bytes from data.
        if (!avail_in) {
            size_t n = (INPUT_BUFFER_SIZE > infile_remaining) ? infile_remaining : INPUT_BUFFER_SIZE; 

            memcpy(&in_data_buffer[0], &chunks.chunks[1].data[chunks.chunks[1].size-infile_remaining], n);
            
            next_in = in_data_buffer;
            avail_in = n;

            infile_remaining -= n;

            #ifdef PNG_DEBUG
                log_v("Copied %d bytes to buffer.", n);
            #endif

            log_v("%d", in_data_buffer);
        }

        in_bytes = avail_in;
        out_bytes = avail_out;
        status = tinfl_decompress(&inflator, (const mz_uint8 *)next_in, &in_bytes, out_data_buffer, (mz_uint8 *)next_out, &out_bytes, (infile_remaining ? TINFL_FLAG_HAS_MORE_INPUT : 0) | TINFL_FLAG_PARSE_ZLIB_HEADER);

        avail_in -= in_bytes;
        next_in = (const mz_uint8 *)next_in + in_bytes;
        total_in += in_bytes;

        avail_out -= out_bytes;
        next_out = (mz_uint8 *)next_out + out_bytes;
        total_out += out_bytes;
        

        if ((status <= TINFL_STATUS_DONE) || (!avail_out)) {
            // Output buffer is full, or decompression is done, so write buffer to output file.
            memcpy(&png_data[total_out], &out_data_buffer[0], OUTPUT_BUFFER_SIZE - (uint)avail_out);

            next_out = out_data_buffer;
            avail_out = OUTPUT_BUFFER_SIZE;
        }

        // If status is <= TINFL_STATUS_DONE then either decompression is done or something went wrong.
        if (status <= TINFL_STATUS_DONE) {
            if (status == TINFL_STATUS_DONE) {
                // Decompression completed successfully.
                is_finished = true;
            }
            else {
                // Decompression failed.
                log_e("Error: tinfl_decompressed failed. (%i)", status);
                return false;
            }
        }
    }
    */

    tinfl_decompressor inflator;
    tinfl_init(&inflator);

    size_t in_bytes = chunks.chunks[1].size;
	size_t out_bytes;
    uint8_t *next_out;

    unsigned char *png_data = (unsigned char *)ps_calloc(((header.width * (header.height * bpp + 7)) / 8) + header.height, sizeof(unsigned char));

    tinfl_status status = tinfl_decompress(&inflator, (const mz_uint8 *)chunks.chunks[1].data, &in_bytes, png_data, (mz_uint8 *)next_out, &out_bytes, TINFL_FLAG_HAS_MORE_INPUT | TINFL_FLAG_PARSE_ZLIB_HEADER);

    if (status < TINFL_STATUS_DONE) {
		// Decompression failed.
        log_e("Error: tinfl_decompressed failed. (%i)", status);
        return false;
	}

    return true;
}

// FILE READING FUNCTIONS //

rawPNGFile png_decode::readFile(fs::FS &fs, const char *filename) {
    rawPNGFile png;
    File pngFile;

    // Attempt to open the file.
    #ifdef PNG_DEBUG
        log_v("Opening %s...", filename);
    #endif
    pngFile = fs.open(filename);
    if (!pngFile || pngFile.isDirectory()) {
        #ifdef PNG_DEBUG
            log_w("File %s doesn't exist.", filename);
        #endif
        pngFile.close();
        png.valid = false;
        return png;
    }

    // Get the size of the file in bytes.
    png.size = pngFile.size();
    #ifdef PNG_DEBUG
        log_v("Size: %d", png.size);
    #endif

    // Allocate array to hold the file.
    png.data = (uint8_t *)ps_calloc(png.size, sizeof(char));
    if (png.data == nullptr) {
        #ifdef PNG_DEBUG
            log_w("Failed to allocate data array in PSRAM.");
        #endif

        pngFile.close();
        png.valid = false;
        return png;
    } else {
        // Read the file into the buffer in PSRAM.
        #ifdef PNG_DEBUG
            log_v("Reading data in.");
        #endif

        pngFile.read(png.data, png.size);
        pngFile.close();

        #ifdef PNG_DEBUG
            log_v("Finished reading file.");
        #endif
    }

    return png;
}

// CHUNK PARSING FUNCTIONS //

PNGChunks png_decode::readChunks(const rawPNGFile png) {
    PNGChunks chunks;
    bool end = false;
    size_t currentByte = 8;

    chunks.num = 0;

    while (!end) {
        chunks.num++;
        #ifdef PNG_DEBUG
            log_v("Chunk %d:", chunks.num);
        #endif
        
        // resize array
        if (chunks.chunks == nullptr) {
            #ifdef PNG_DEBUG
                log_v("No chunk array found, allocating 1 member.");
            #endif
            chunks.chunks = (PNGChunk *)calloc(chunks.num, sizeof(PNGChunk));
            if (chunks.chunks == nullptr) {
                #ifdef PNG_DEBUG
                    log_w("Failed to allocate chunk array in SRAM.");
                #endif
                chunks.valid = false;
                return chunks;
            }
            #ifdef PNG_DEBUG
                log_v("Allocated chunk array in SRAM.");
            #endif
        } else {
            #ifdef PNG_DEBUG
                log_v("Chunk array found, adding a new member.");
            #endif
            // create new array
            chunks.chunks = (PNGChunk *)realloc(chunks.chunks, chunks.num * sizeof(PNGChunk));

            if (!chunks.chunks) {
                #ifdef PNG_DEBUG
                    log_w("Failed to reallocate chunk array in SRAM.");
                #endif
                chunks.valid = false;
                return chunks;
            } else {
                #ifdef PNG_DEBUG
                    log_v("Reallocated chunk array in SRAM.");
                #endif
            }
        }
        chunks.chunks[chunks.num-1] = readChunk(&png.data[currentByte]);
        currentByte += 8+chunks.chunks[chunks.num-1].size+4;
        if (currentByte >= png.size-11) end = true;
    }

    log_v("Total chunks: %d", chunks.num);
    return chunks;
}

/*
Chunk Structure:

<size><name><data><crc>
<-4--><-4--><size><-4->

size:
    size of data

name:
    used to decode file by infering what the data is

data:
    data chunk of the size given in the first 4 bytes

crc:
    checksum to validate data integrity.
*/

PNGChunk png_decode::readChunk(const uint8_t *data) {
    PNGChunk newChunk;

    // Read in the size
    newChunk.size = read32BitInt(&data[0]);
    #ifdef PNG_DEBUG
        log_v("Size: %d", newChunk.size);
    #endif

    // Store the name in SRAM
    newChunk.name = (char *)calloc(4, sizeof(char)); 
    if (newChunk.name == nullptr) {
        #ifdef PNG_DEBUG
            log_w("Failed to allocate chunk name in SRAM.");
        #endif
        newChunk.valid = false;
        return newChunk;
    } else {
        #ifdef PNG_DEBUG
            log_v("Allocated chunk name in SRAM.");
        #endif
        memcpy(&newChunk.name[0], &data[4], 4);
        #ifdef PNG_DEBUG
            log_v("Name: %.4s", newChunk.name);
        #endif
    }

    // Store the data in PSRAM
    if (newChunk.size != 0) {
        newChunk.data = (unsigned char *)ps_calloc(newChunk.size, sizeof(char));
        if (newChunk.data == nullptr) {
            #ifdef PNG_DEBUG
                log_w("Failed to allocate data in PSRAM.");
            #endif
            newChunk.valid = false;
            return newChunk;
        } else {
            #ifdef PNG_DEBUG
                log_v("Allocated data in PSRAM.");
            #endif
            memcpy(&newChunk.data[0], &data[8], newChunk.size);
        }
    } else {
        #ifdef PNG_DEBUG
            log_v("No data to allocate.");
        #endif
    }

    // Read in crc
    newChunk.crc = read32BitInt(&data[8+newChunk.size]);
    #ifdef PNG_DEBUG
        log_v("CRC: %d", newChunk.crc);
    #endif

    return newChunk;
}

// CHUNK CHECKING FUNCTIONS //

/*
PNG File Signature:

<----------------------------Signature------------------------------->
<8-bit Checker><PNG Identification><DOS CRLF><Type End><Unix Line End>
<----0x89-----><-----0x504E47-----><-0x0D0A-><--0x1A--><----0x0A----->

8-bit Checker:
    Used to detect systems that don't support 8-bit data.
PNG Identification:
    0x504E47 will appear as PNG in a hex editor and can be used to identify the file as such.
DOS CRLF:
    DOS line end.
Type End:
    End of file character for the "type" command, as all data about the type has been given.
Unix Line End:
    Unix line end.

*/

bool png_decode::checkSignature(const uint8_t *data) {
    // Proper PNG file format signature
    if (data[0] == 137 && data[1] == 'P' && data[2] == 'N' && data[3] == 'G' && data[4] == 13 && data[5] == 10 && data[6] == 26 && data[7] == 10) {
        #ifdef PNG_DEBUG
            log_v("Valid file signature.");
        #endif
        return true;
    }

    #ifdef PNG_DEBUG
        log_w("Invalid file signature.");
    #endif
    return false;
}

/*
The following is the structure of a png image header.
More info can be found at http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.IHDR.

<----------------------HEADER FORMAT------------------------>
<Width><Height><Depth><Color><Compression><Filter><Interlace>
<--4--><--4---><--1--><--1--><-----1-----><--1---><----1---->
<--------------------------bytes---------------------------->

<Width>
    The width of the image.

<Height>
    The height of the image.

<Depth>
    The bit depth is based on what combinations work best for the specified color type.

<Color>
    Color type represents how to interpret the image.

<Compression>
    Currently the only supported method is 0.
    http://www.libpng.org/pub/png/spec/1.2/PNG-Compression.html

<Filter>
    Algorithm used to prepare image data for compression.
    http://www.libpng.org/pub/png/spec/1.2/PNG-Filters.html

<Interlace>
    0 for none, 1 for "Adam7 interlace"
    http://www.libpng.org/pub/png/spec/1.2/PNG-DataRep.html#DR.Interlaced-data-order
*/

PNGHeader png_decode::readHeader(const PNGChunk header) {
    PNGHeader newHeader;

    #ifdef PNG_DEBUG
        log_v("Header Chunk:");
    #endif

    // Ensure chunk is header chunk
    if (header.name[0] != 'I' || header.name[1] != 'H' || header.name[2] != 'D' || header.name[3] != 'R') {
        #ifdef PNG_DEBUG
            log_w("Expected header chunk, got %.4s isntead.", header.name);
        #endif
        newHeader.valid = false;
        return newHeader;
    }

    // Ensure chunk is expected size
    if (header.size != 13) {
        #ifdef PNG_DEBUG
            log_w("Header chunk size should be 13 bytes, got %d instead.", header.size);
        #endif
        newHeader.valid = false;
        return newHeader;
    }

    // Width/Height
    newHeader.width = read32BitInt(&header.data[0]);
    #ifdef PNG_DEBUG
        log_v("Width: %d", newHeader.width);
    #endif
    newHeader.height = read32BitInt(&header.data[4]);
    #ifdef PNG_DEBUG
        log_v("Height: %d", newHeader.height);
    #endif

    // bitDepth/colorType
    newHeader.bitDepth = header.data[8];
    #ifdef PNG_DEBUG
        log_v("Bit Depth: %d", newHeader.bitDepth);
    #endif
    newHeader.colorType = header.data[9];
    #ifdef PNG_DEBUG
        log_v("Color Type: %d", newHeader.colorType);
    #endif

    if (newHeader.colorType == 0) {
        if (newHeader.bitDepth != 1 && newHeader.bitDepth != 2 && newHeader.bitDepth != 4 && newHeader.bitDepth != 8 && newHeader.bitDepth != 16) {
            #ifdef PNG_DEBUG
                log_w("Invalid colorType/bitDepth combination of %d and %d respectively.", newHeader.colorType, newHeader.bitDepth);
            #endif
            newHeader.valid = false;
            return newHeader;
        }
    } else if (newHeader.colorType == 2 || newHeader.colorType == 4 || newHeader.colorType == 6) {
        if (newHeader.bitDepth != 8 && newHeader.bitDepth != 16) {
            #ifdef PNG_DEBUG
                log_w("Invalid colorType/bitDepth combination of %d and %d respectively.", newHeader.colorType, newHeader.bitDepth);
            #endif
            newHeader.valid = false;
            return newHeader;
        }
    } else if (newHeader.colorType == 3) {
        if (newHeader.bitDepth != 1 && newHeader.bitDepth != 2 && newHeader.bitDepth != 4 && newHeader.bitDepth != 8) {
            #ifdef PNG_DEBUG
                log_w("Invalid colorType/bitDepth combination of %d and %d respectively.", newHeader.colorType, newHeader.bitDepth);
            #endif
            newHeader.valid = false;
            return newHeader;
        }
    } else {
        #ifdef PNG_DEBUG
            log_w("Color type \"%d\" is invalid.", newHeader.colorType);
        #endif
        newHeader.valid = false;
        return newHeader;
    }

    // Compression
    if (header.data[10] != 0) {
        #ifdef PNG_DEBUG
            log_w("Invalid compression type for the png format standard.");
        #endif
        newHeader.valid = false;
        return newHeader;
    }
    
    
    // Filter
    newHeader.filterType = header.data[11];

    if (newHeader.filterType < 0 && newHeader.filterType > 4) {
        #ifdef PNG_DEBUG
            log_w("Invalid filter type, expected value between 0 and 4, got %d instead.", newHeader.filterType);
        #endif
        newHeader.valid = false;
        return newHeader;
    }

    #ifdef PNG_DEBUG
        switch (newHeader.filterType) {
            case 0:
                log_v("Filter Type: None");
                break;
            case 1:
                log_v("Filter Type: Sub");
                break;
            case 2:
                log_v("Filter Type: Up");
                break;
            case 3:
                log_v("Filter Type: Average");
                break;
            case 4:
                log_v("Filter Type: Paeth");
                break;
            default:
                break;
        }
    #endif

    // Interlace
    switch (header.data[12]) {
        case 0:
            newHeader.interlaceAdam7 = false;
            break;
        case 1:
            newHeader.interlaceAdam7 = true;
            break;
        default:
            #ifdef PNG_DEBUG
                log_w("Invalid interlace type, expected either 0 or 1, got %d instead.", newHeader.interlaceAdam7);
            #endif
            newHeader.valid = false;
            return newHeader;
    }

    #ifdef PNG_DEBUG
        switch (static_cast<int>(newHeader.interlaceAdam7)) {
            case 0:
                log_v("Interlace Type: None");
                break;
            case 1:
                log_v("Interlace Type: Adam7");
                break;
            default:
                break;
        }
    #endif

    newHeader.valid = true;
    return newHeader;
}

/*
See http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.Summary-of-standard-chunks

Chunk Structure:

~ = isn't neccessary
* = can be anywhere between IHDR and IEND
& = can be multiple

IHDR
~pHYs sPLT&
  ~cHRM gAMA iCCP sBIT sRGB bKGD
~PLTE
  ~bKGD hIST tRNS
IDAT
*tIME iTXt& tEXt& zTXt&
IEND

*/

/*

Critical Chunks:

IHDR:
    needed header with information about the image

PLTE:
    optional color palette

IDAT:
    image data

IEND:
    needed end of the file

Ancillary Chunks:

tRNS:
    simple transparancy data

gAMA:
    display output intensity

cHRM:
    device-independent color specification

sRGB:
    conform to the sRGB color space

iCCP:
    embedded ICC color profile

iTXt, tEXt, zTXt:
    different ways to associate text info

bKGD:
    background color

pHYs:
    individual pixel dimensions

sBIT:
    sample bit depth

sPLT:
    suggested fallback palette

hIST:
    frequency of colors used in the palette

tIME:
    time of last image modification

*/


//bool png_decode::checkChunks(const PNGChunks png) {

//}

// HELPER FUNCTIONS //

/*
PNGChunks png_decode::removeChunk(PNGChunks chunks, const uint8_t num) {
    PNGChunks newChunks;

    // remove one chunk from the chunk count.
    newChunks.num = chunks.num - 1;

    if (num > chunks.num || num <= 0) {
        #ifdef PNG_DEBUG
            log_w("Chunk to remove was out of bounds.");
        #endif
        newChunks.valid = false;
        return newChunks;
    }

    // free data from removal chunk
    #ifdef PNG_DEBUG
        log_v("Clearing chunk data.");
    #endif

    // If a name was allocated, free the name
    if (chunks.chunks[chunks.num-1].name != nullptr) {
        free(chunks.chunks[chunks.num-1].name);
        chunks.chunks[chunks.num-1].name = nullptr;
    }
    
    // If data was allocated, free the data
    if (chunks.chunks[chunks.num-1].data != nullptr) {
        free(chunks.chunks[chunks.num-1].data);
        chunks.chunks[chunks.num-1].data = nullptr;
    }

    // Reset non-pointer values.
    chunks.chunks[chunks.num-1].size = 0;
    chunks.chunks[chunks.num-1].crc = 0;
    chunks.chunks[chunks.num-1].valid = true;

    if (num == chunks.num) {
        #ifdef PNG_DEBUG
            log_v("Removing ending chunk.");
        #endif

        newChunks.chunks = (PNGChunk *)realloc(chunks.chunks, newChunks.num * sizeof(PNGChunk));
    } else if (num == 1) {
        #ifdef PNG_DEBUG
            log_v("Removing first chunk.");
        #endif

        memmove(chunks.chunks, &(chunks.chunks[1]), newChunks.num * sizeof(PNGChunk));
        newChunks.chunks = (PNGChunk *)realloc(chunks.chunks, newChunks.num * sizeof(PNGChunk));
    }/* else {

    }*/    
/*
    if (!newChunks.chunks) {
        #ifdef PNG_DEBUG
            log_w("Failed to reallocate chunk array in SRAM.");
        #endif
        newChunks.valid = false;
        return newChunks;
    } else {
        #ifdef PNG_DEBUG
            log_v("Reallocated chunk array in SRAM.");
        #endif
    }

    return newChunks;
}
*/

// As of right now doesn't support grayscale.
uint8_t png_decode::calcBpp(const uint8_t colorType, const uint8_t bitDepth) {
    if (colorType == 2) {
        #ifdef PNG_DEBUG
            log_v("BPP: %d", 3*bitDepth);
        #endif
        return (3*bitDepth);
    } else {
        #ifdef PNG_DEBUG
            log_v("BPP: %d", bitDepth);
        #endif
        return bitDepth;
    }
}

bool png_decode::isEndChunk(const PNGChunk endChunk) {
    if (endChunk.name[0] != 'I' || endChunk.name[1] != 'E' || endChunk.name[2] != 'N' || endChunk.name[3] != 'D') {
        #ifdef PNG_DEBUG
            log_w("Expected end chunk, got %.4s instead.", endChunk.name);
        #endif
        return false;
    }

    #ifdef PNG_DEBUG
        log_v("Valid end chunk.");
    #endif
    return true;
}

uint32_t png_decode::read32BitInt(const uint8_t *data) {
    return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
}
