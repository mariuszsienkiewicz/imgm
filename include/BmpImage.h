#ifndef BMPIMAGE_H
#define BMPIMAGE_H

#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include "Image.h"
#include "PixelManager.h"

/**
 * Struct of the Pixel that is used in the BMP image file format.
 */
struct RGB {
    uint8_t b;
    uint8_t g;
    uint8_t r;
};

/**
 * Main BMP Image header class.
 */
class BmpImage : public Image, public PixelManager<RGB> {

private:

    /**
     * BMP file header struct - containing all of the necessary file data.
     */
    struct bmp_file_header {
        uint32_t size;
        uint16_t reserved;
        uint16_t reservedSecond;
        uint32_t offset;
    };

    /**
     * BMP info header struct - containing all of the necessary image data.
     */
    struct bmp_info_header {
        uint32_t headerSize;
        int32_t width;
        int32_t height;
        uint16_t planes;
        uint16_t bitsPerPixel;
        uint32_t compression;
        uint32_t imageSize;
        uint32_t xPixelsPerM;
        uint32_t yPixelsPerM;
        uint32_t colorsUsed;
        uint32_t colorsImportant;
    };

    bmp_file_header * bmpFileHeader;
    bmp_info_header * bmpInfoHeader;

    /**
     * Contains all the data after the end of known and understandable information.
     * It's used to make more files compatible.
     */
    std::vector<uint8_t> * restOfTheFile = new std::vector<uint8_t>;

    /**
     * Contains the information if the image is in the binary format.
     */
    bool isBinary = false;

    // Additional file methods

    /**
     * Reads the header file and passes all the information to the bmp_file_header struct.
     * @param file - image file stream
     */
    void readHeaderFile();

    /**
     * Reads the image information header and passes all the information to the bmp_info_header.
     * @param file - image file stream
     */
    void readInfoHeader();

    /**
     * Reads the color table (all of the pixels) to the image variable.
     * @param file - image file stream
     */
    void readColorTable();

    /**
     * Reads the rest of the file to the restOfTheFile variable
     * @param file - image file stream
     */
    void readRestOfTheFile();

    /**
     * Recalculates the size of image.
     * @return - size of the image in uint32_t datatype
     */
    uint32_t getRecalculatedSizeOfImage();

    // override

    bool checkSignature() override;

public:

    /**
     * Performs validation and reads the image.
     * @param file - fstream of the image file.
     */
    explicit BmpImage(std::fstream& file);

    /**
     * Creates the grayscale.
     */
    void toGrayscale();

    // override

    void validate() override;
    void read() override;
    void save(std::string path) const override;

    void blur() override;
    void toBinary(int threshold) override;
    void erode() override;
    void dilate() override;
    void toNegative() override;
    void scale(int newWidth, int newHeight) override;
    void scaleDown(int newWidth, int newHeight) override;
    void scaleUp(int newWidth, int newHeight) override;
    void edgeFilter() override;
    void denoise(int size) override;
    void rotate(float degree) override;
};

#endif //BMPIMAGE_H
