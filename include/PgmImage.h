#ifndef PGMIMAGE_H
#define PGMIMAGE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include "Image.h"
#include "PixelManager.h"

/**
 * PGM (portable graymap format) class.
 * Uses uint8_t datatype pixel representation that is passed to the PixelManager class.
 */
class PgmImage : public Image, public PixelManager<uint8_t> {

    /**
     * Char values representing whitespaces
     */
    std::vector<char> * whitespaces = new std::vector<char>{10, 13, 32, 9};

    /**
     * Width of the image - in pixels
     */
    int width;

    /**
     * Height of the image - in pixels
     */
    int height;

    /**
     * Max value representing the colour
     */
    uint8_t maxVal;

    /**
     * Contains the information if the image is in the binary format
     */
    bool isBinary;

    // Utils
    /**
     * Reads the char values to the next whitespace from the file stream
     * @return
     */
    std::vector<char> readToTheNextWhitespace();

    // Additional file methods

    /**
     * Reads the file info header and passing the values to the width, height, maxVal class members.
     */
    void readInfoHeader();

    /**
     * Skips the next byte.
     */
    void skipNextByte();

    /**
     * Reads the Pixels from the fstream
     */
    void readPixels();

    // override
    bool checkSignature() override;

public:
    explicit PgmImage(std::fstream& file);

    // override
    // ImageReader

    void validate() override;
    void read() override;
    void save(std::string path) const override;

    // Image processing

    void blur() override;
    void toBinary(int threshold) override;
    void erode() override;
    void dilate() override;
    void toNegative() override;
    void scale(int newWidth, int newHeight) override;
    void scaleUp(int newWidth, int newHeight) override;
    void scaleDown(int newWidth, int newHeight) override;
    void edgeFilter() override;
    void denoise(int size) override;
    void rotate(float degree) override;
};

#endif //PGMIMAGE_H
