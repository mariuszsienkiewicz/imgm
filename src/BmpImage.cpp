#include "BmpImage.h"
#include "Tools.h"

BmpImage::BmpImage(std::fstream & fileStream) : PixelManager() {
    this->setFileStream(fileStream);
}

void BmpImage::validate() {
    if (this->getFileStream()->fail()) {
        throw OpeningTheFileException();
    }

    if (!this->checkSignature()) {
        throw WrongFileFormatException();
    }
}

void BmpImage::read() {
    this->validate();

    readHeaderFile();
    readInfoHeader();

    if (this->bmpInfoHeader->headerSize != 40) {
        std::cout << "!============!" << std::endl;
        std::cout << "WARNING, THIS IMAGE USES A HEADER THAT IS NOT SUPPORTED BY THIS PROGRAM." << std::endl;
        std::cout << "IT IS VERY LIKELY THAT THERE WILL BE SOME MAJOR PROBLEMS WITH THIS IMAGE." << std::endl;
        std::cout << "!============!" << std::endl;
    }

    readColorTable();
    readRestOfTheFile();

    this->closeFileStream();
}

/**
 * Rotation without aliasing.
 *
 * @param degree - degree in decimal number which is later changed to radians
 */
void BmpImage::rotate(float degree) {
    auto * modifiedImg = new RGB[this->bmpInfoHeader->width * this->bmpInfoHeader->height];

    int index = 0;
    for (int y = 0; y < this->bmpInfoHeader->height; y++) {
        for (int x = 0; x < this->bmpInfoHeader->width; x++) {
            modifiedImg[index] = RGB{0,0,0};
            index++;
        }
    }

    double xCenter = this->bmpInfoHeader->width / 2.0;
    double yCenter = this->bmpInfoHeader->height / 2.0;
    double cos = std::cos(-degree * (PI/180.0));
    double sin = std::sin(-degree * (PI/180.0));

    for (int row = 0; row < this->bmpInfoHeader->height; row++) {
        for (int column = 0; column < this->bmpInfoHeader->width; column++) {
            RGB pixel = this->getPixelAt(column, row, this->bmpInfoHeader->width);

            double xOffset = column - xCenter;
            double yOffset = row - yCenter;
            int newPosX = (int) (xOffset * cos + yOffset * sin + xCenter);
            int newPosY = (int) (yOffset * cos - xOffset * sin + yCenter);

            if ((newPosX >= 0) && (newPosX < this->bmpInfoHeader->width) &&
                (newPosY >= 0) && (newPosY < this->bmpInfoHeader->height)) {
                this->setPixelAt(newPosX, newPosY, this->bmpInfoHeader->width, modifiedImg, pixel);
            }
        }
    }
    this->setPixels(modifiedImg);
}

/**
 * Checks the signature of the file.
 * @return bool
 */
bool BmpImage::checkSignature() {
    char header[] = {'B', 'M'};
    char actualHeader[2];

    this->getFileStream()->read(actualHeader, 2);

    return std::equal(header, header + 2, actualHeader);
}

/**
 * Reads the header file to the bmp_file_header structure.
 */
void BmpImage::readHeaderFile() {
    this->getFileStream()->seekg(2);
    bmpFileHeader = new bmp_file_header;
    this->getFileStream()->read(reinterpret_cast<char *>(bmpFileHeader), sizeof(bmp_file_header));
}

/**
 * Reads the information header of the file into the bmp_info_header structure.
 */
void BmpImage::readInfoHeader() {
    bmpInfoHeader = new bmp_info_header;
    this->getFileStream()->read(reinterpret_cast<char *>(bmpInfoHeader), sizeof(bmp_info_header));
}

/**
 * Applies blur filter (average filter) to the image.
 * It skips the border pixels.
 */
void BmpImage::blur() {
    auto * modifiedImg = new RGB[this->bmpInfoHeader->width * this->bmpInfoHeader->height];

    int index = 0;
    for (int row = 0; row < this->bmpInfoHeader->height; row++) {
        for (int column = 0; column < this->bmpInfoHeader->width; column++) {
            if (row == 0 || column == 0 || column == this->bmpInfoHeader->width - 1 ||
                row == this->bmpInfoHeader->height - 1) {
                modifiedImg[index] = this->getPixelAt(column, row, this->bmpInfoHeader->width);
            } else {
                RGB a = this->getPixelAt(column - 1, row - 1, this->bmpInfoHeader->width);
                RGB b = this->getPixelAt(column - 1, row, this->bmpInfoHeader->width);
                RGB c = this->getPixelAt(column - 1, row + 1, this->bmpInfoHeader->width);

                RGB d = this->getPixelAt(column, row - 1, this->bmpInfoHeader->width);
                RGB x = this->getPixelAt(column, row, this->bmpInfoHeader->width);
                RGB e = this->getPixelAt(column, row + 1, this->bmpInfoHeader->width);

                RGB f = this->getPixelAt(column + 1, row - 1, this->bmpInfoHeader->width);
                RGB g = this->getPixelAt(column + 1, row, this->bmpInfoHeader->width);
                RGB h = this->getPixelAt(column - 1, row + 1, this->bmpInfoHeader->width);

                uint8_t bAvg = (a.b + b.b + c.b + d.b + x.b + e.b + f.b + g.b + h.b) / 9;
                uint8_t gAvg = (a.g + b.g + c.g + d.g + x.g + e.g + f.g + g.g + h.g) / 9;
                uint8_t rAvg = (a.r + b.r + c.r + d.r + x.r + e.r + f.r + g.r + h.r) / 9;

                RGB modified = {bAvg, gAvg, rAvg};
                modifiedImg[index] = modified;
            }
            index++;
        }
    }

    this->setPixels(modifiedImg);
}

void BmpImage::toGrayscale() {
    auto * modifiedImg = new RGB[this->bmpInfoHeader->width * this->bmpInfoHeader->height];

    int index = 0;
    for (int row = 0; row < this->bmpInfoHeader->height; row++) {
        for (int column = 0; column < this->bmpInfoHeader->width; column++) {
            RGB pixel = this->getPixelAt(column, row, this->bmpInfoHeader->width);

            uint8_t grayscale = pixel.r / 3 + pixel.g / 3 + pixel.b / 3;

            RGB newPixel = RGB{grayscale, grayscale, grayscale};
            modifiedImg[index] = newPixel;
            index++;
        }
    }

    this->setPixels(modifiedImg);
}

/**
 * Creating a binary image using threshold given by the parameter.
 */
void BmpImage::toBinary(int threshold) {
    // create grayscale image, it will be easier to work with it
    toGrayscale();

    // create binary image
    int index = 0;
    auto *modifiedImg = new RGB[this->bmpInfoHeader->width * this->bmpInfoHeader->height];
    for (int row = 0; row < this->bmpInfoHeader->height; row++) {
        for (int column = 0; column < this->bmpInfoHeader->width; column++) {
            RGB pixel = this->getPixelAt(column, row, (int) this->bmpInfoHeader->width);
            RGB newPixel{};
            if (pixel.r <= threshold) {
                newPixel = RGB{0, 0, 0};
            } else {
                newPixel = RGB{255, 255, 255};
            }

            modifiedImg[index] = newPixel;
            index++;
        }
    }
    this->setPixels(modifiedImg);
    isBinary = true;
}

/**
 * Performs erode operation on image (only if it is in the binary format).
 */
void BmpImage::erode() {
    if (!isBinary) {
        throw NotInBinaryFormatException();
    }

    int radius = 3;

    // create binary image
    int index = 0;
    auto *modifiedImg = new RGB[this->bmpInfoHeader->width * this->bmpInfoHeader->height];
    for (int row = 0; row < this->bmpInfoHeader->height; row++) {
        int topEdge = std::min(row + radius, this->bmpInfoHeader->height - 1);
        int bottomEdge = std::max(row - radius, 0);

        for (int column = 0; column < this->bmpInfoHeader->width; column++) {
            RGB actualPixel = this->getPixelAt(column, row, this->bmpInfoHeader->width);
            int leftEdge = std::max(column - radius, 0);
            int rightEdge = std::min(column + radius, this->bmpInfoHeader->width - 1);
            bool toErode = false;

            for (int y = bottomEdge; y <= topEdge; y++) {
                for (int x = leftEdge; x <= rightEdge; x++) {
                    RGB pixel = this->getPixelAt(x, y, this->bmpInfoHeader->width);
                    if (pixel.r == 0 && pixel.g == 0 && pixel.b == 0) {
                        toErode = true;
                    }
                }
            }

            if (toErode) {
                actualPixel = RGB{0, 0, 0};
            }

            modifiedImg[index] = actualPixel;
            index++;
        }
    }

    this->setPixels(modifiedImg);
}

/**
 * Performs dilatation operation on the image (only if it is in the binary format)
 */
void BmpImage::dilate() {
    if (!isBinary) {
        throw NotInBinaryFormatException();
    }

    int radius = 2;

    // create binary image
    auto *modifiedImg = new RGB[this->bmpInfoHeader->width * this->bmpInfoHeader->height];
    int index = 0;
    for (int row = 0; row < this->bmpInfoHeader->height; row++) {
        int topEdge = std::min(row + radius, this->bmpInfoHeader->height - 1);
        int bottomEdge = std::max(row - radius, 0);

        for (int column = 0; column < this->bmpInfoHeader->width; column++) {
            RGB actualPixel = this->getPixelAt(column, row, this->bmpInfoHeader->width);
            int leftEdge = std::max(column - radius, 0);
            int rightEdge = std::min(column + radius, this->bmpInfoHeader->width - 1);
            bool toErode = false;

            for (int y = bottomEdge; y <= topEdge; y++) {
                for (int x = leftEdge; x <= rightEdge; x++) {
                    RGB pixel = this->getPixelAt(x, y, this->bmpInfoHeader->width);
                    if (pixel.r == 255 && pixel.g == 255 && pixel.b == 255) {
                        toErode = true;
                    }
                }
            }

            if (toErode) {
                actualPixel = RGB{255, 255, 255};
            }

            modifiedImg[index] = actualPixel;
            index++;
        }
    }

    this->setPixels(modifiedImg);
}

void BmpImage::toNegative() {
    auto * modifiedImg = new RGB[this->bmpInfoHeader->width * this->bmpInfoHeader->height];
    int index = 0;
    for (int row = 0; row < this->bmpInfoHeader->height; row++) {
        for (int column = 0; column < this->bmpInfoHeader->width; column++) {
            RGB pixel = this->getPixelAt(column, row, this->bmpInfoHeader->width);
            RGB modified = RGB{};
            modified.b = 255 - pixel.b;
            modified.g = 255 - pixel.g;
            modified.r = 255 - pixel.r;

            modifiedImg[index] = modified;
            index++;
        }
    }
    this->setPixels(modifiedImg);
}

void BmpImage::scale(int width, int height) {
    if (width > this->bmpInfoHeader->width || height > this->bmpInfoHeader->height) {
        scaleUp(width, height);
    } else {
        scaleDown(width, height);
    }
}

void BmpImage::scaleUp(int width, int height) {
    double scaleWidth = (double) this->bmpInfoHeader->width / width;
    double scaleHeight = (double) this->bmpInfoHeader->height / height;

    auto * modifiedImg = new RGB[width * height];
    int index = 0;
    for (int row = 0; row < height; row++) {
        for (int column = 0; column < width; column++) {
            RGB pixel = this->getPixelAt(column * scaleWidth, row * scaleHeight, this->bmpInfoHeader->width);
            modifiedImg[index] = pixel;
            index++;
        }
    }

    this->setPixels(modifiedImg);

    this->bmpInfoHeader->width = (int32_t) width;
    this->bmpInfoHeader->height = (int32_t) height;
    this->bmpFileHeader->size = getRecalculatedSizeOfImage();
}

void BmpImage::scaleDown(int width, int height) {
    double scaleWidth = (double) width / (double) this->bmpInfoHeader->width;
    double scaleHeight = (double) height / (double) this->bmpInfoHeader->height;
    int boxWidth = (int) std::ceil(1 / scaleWidth);
    int boxHeight = (int) std::ceil(1 / scaleHeight);

    auto *modifiedImg = new RGB[width * height];
    int index = 0;
    for (int row = 0; row < height; row++) {
        for (int column = 0; column < width; column++) {
            int xStartOriginal = std::floor(column / scaleWidth);
            int yStartOriginal = std::floor(row / scaleHeight);
            int xStopOriginal = std::min(xStartOriginal + boxWidth, this->bmpInfoHeader->width - 1);
            int yStopOriginal = std::min(yStartOriginal + boxHeight, this->bmpInfoHeader->height - 1);

            int redSum = 0;
            int greenSum = 0;
            int blueSum = 0;
            int count = 0;
            for (int y = yStartOriginal; y <= yStopOriginal; y++) {
                for (int x = xStartOriginal; x <= xStopOriginal; x++) {
                    RGB pixel = this->getPixelAt(x, y, this->bmpInfoHeader->width);
                    redSum += pixel.r;
                    greenSum += pixel.g;
                    blueSum += pixel.b;
                    count++;
                }
            }
            uint8_t calculatedBlue = blueSum / count;
            uint8_t calculatedGreen = greenSum / count;
            uint8_t calculatedRed = redSum / count;

            RGB newPixel = RGB{calculatedBlue, calculatedGreen, calculatedRed};
            modifiedImg[index] = newPixel;
            index++;
        }
    }

    this->setPixels(modifiedImg);

    this->bmpInfoHeader->width = (int32_t) width;
    this->bmpInfoHeader->height = (int32_t) height;
    this->bmpFileHeader->size = getRecalculatedSizeOfImage();
}

/**
 * Edge filter using the Sobel operator
 */
void BmpImage::edgeFilter() {
    toGrayscale();

    // sobel operator
    int kernelSize = 3; // kernel convolution matrix size
    int Gx[3][3] = {
            {1, 0, -1},
            {2, 0, -2},
            {1, 0, -1}
    };
    int Gy[3][3] = {
            {1, 2, 1},
            {0, 0, 0},
            {-1, -2, -1}
    };

    auto * modifiedImg = new RGB[this->bmpInfoHeader->width * this->bmpInfoHeader->height];
    int accumulatorXR, accumulatorXG, accumulatorXB, accumulatorYR, accumulatorYG, accumulatorYB;
    int index = 0;
    for (int row = 0; row < this->bmpInfoHeader->height; row++) {
        int topEdge = std::min(row + kernelSize, this->bmpInfoHeader->height - 1);
        int bottomEdge = std::max(row - kernelSize, 0);

        for (int column = 0; column < this->bmpInfoHeader->width; column++) {
            int leftEdge = std::max(column - kernelSize, 0);
            int rightEdge = std::min(column + kernelSize, this->bmpInfoHeader->width - 1);

            int indexMatrixRGBs = 0;
            RGB matrixRGBs[(topEdge - bottomEdge + 1) * (rightEdge - leftEdge + 1)];
            for (int y = bottomEdge; y <= topEdge; y++) {
                for (int x = leftEdge; x <= rightEdge; x++) {
                    matrixRGBs[indexMatrixRGBs] = this->getPixelAt(x, y, this->bmpInfoHeader->width);
                    indexMatrixRGBs++;
                }
            }

            // Accumulator X for each RGB channel
            accumulatorXR = 0, accumulatorXG = 0, accumulatorXB = 0;
            // Accumulator Y for each RGB channel
            accumulatorYR = 0, accumulatorYG = 0, accumulatorYB = 0;
            for (int gY = 0; gY < kernelSize; gY++) {
                for (int gX = 0; gX < kernelSize; gX++) {
                    RGB rgb = this->getPixelAt(gX, gY, kernelSize - 1, matrixRGBs);

                    accumulatorXR += rgb.r * Gx[gY][gX];
                    accumulatorXG += rgb.g * Gx[gY][gX];
                    accumulatorXB += rgb.b * Gx[gY][gX];

                    accumulatorYR += rgb.r * Gy[gY][gX];
                    accumulatorYG += rgb.g * Gy[gY][gX];
                    accumulatorYB += rgb.b * Gy[gY][gX];
                }
            }

            uint8_t resultR = std::min((int) sqrt((accumulatorXR * accumulatorXR) + (accumulatorYR * accumulatorYR)), 255);
            uint8_t resultG = std::min((int) sqrt((accumulatorXG * accumulatorXG) + (accumulatorYG * accumulatorYG)), 255);
            uint8_t resultB = std::min((int) sqrt((accumulatorXB * accumulatorXB) + (accumulatorYB * accumulatorYB)), 255);

            RGB rgb = RGB{resultB, resultG, resultR};
            modifiedImg[index] = rgb;
            index++;
        }
    }

    this->setPixels(modifiedImg);
}

void BmpImage::denoise(int size) {
    auto * modifiedImg = new RGB[this->bmpInfoHeader->width * this->bmpInfoHeader->height];
    int radius = (size - 1) / 2;

    int index = 0;
    for (int row = 0; row < this->bmpInfoHeader->height; row++) {
        int topEdge = std::min(row + radius, this->bmpInfoHeader->height - 1);
        int bottomEdge = std::max(row - radius, 0);

        for (int column = 0; column < this->bmpInfoHeader->width; column++) {
            int leftEdge = std::max(column - radius, 0);
            int rightEdge = std::min(column + radius, this->bmpInfoHeader->width - 1);

            size_t arrayOfValuesSize = (topEdge - bottomEdge + 1) * (rightEdge - leftEdge + 1);
            uint8_t rValues[arrayOfValuesSize], gValues[arrayOfValuesSize], bValues[arrayOfValuesSize];
            int matrixIndex = 0;
            for (int y = bottomEdge; y <= topEdge; y++) {
                for (int x = leftEdge; x <= rightEdge; x++) {
                    RGB pixel = this->getPixelAt(x, y, this->bmpInfoHeader->width);

                    rValues[matrixIndex] = pixel.r;
                    gValues[matrixIndex] = pixel.g;
                    bValues[matrixIndex] = pixel.b;

                    matrixIndex++;
                }
            }

            uint8_t r = Tools::median(rValues, arrayOfValuesSize);
            uint8_t g = Tools::median(gValues, arrayOfValuesSize);
            uint8_t b = Tools::median(bValues, arrayOfValuesSize);

            RGB newPixel = RGB{b, g, r};
            modifiedImg[index] = newPixel;
            index++;
        }
    }

    this->setPixels(modifiedImg);
}

uint32_t BmpImage::getRecalculatedSizeOfImage() {
    return 2 +
           sizeof(bmp_info_header) +
           sizeof(bmp_file_header) +
           (sizeof(RGB) * this->bmpInfoHeader->width * this->bmpInfoHeader->height) +
           (sizeof(uint8_t) * this->restOfTheFile->size());
}

void BmpImage::readColorTable() {
    if (this->bmpInfoHeader->bitsPerPixel != 24) {
        throw WrongMetadataException();
    }

    unsigned int padding = 0;
    if (((this->bmpInfoHeader->width * sizeof(RGB)) % 4) != 0) {
        padding = (4 - ((this->bmpInfoHeader->width * sizeof(RGB)) % 4));
    }

    uint8_t blue;
    uint8_t green;
    uint8_t red;

    auto pixelArray = new RGB[this->bmpInfoHeader->width * this->bmpInfoHeader->height];

    this->getFileStream()->seekg(this->bmpFileHeader->offset);
    int count = 0;
    for (int row = 0; row < this->bmpInfoHeader->height; row++) {
        for (int column = 0; column < this->bmpInfoHeader->width; column++) {
            blue = this->getFileStream()->get();
            green = this->getFileStream()->get();
            red = this->getFileStream()->get();

            pixelArray[count] = RGB{blue, green, red};
            count++;
        }

        if (padding != 0) {
            this->getFileStream()->seekg(padding, std::ios_base::cur); // skip padding
        }
    }

    this->setPixels(pixelArray);
}

void BmpImage::readRestOfTheFile() {
    int byte;
    while ((byte = this->getFileStream()->get()) != EOF) {
        this->restOfTheFile->push_back(byte);
    }
}

void BmpImage::save(std::string filename) const {
    using std::ios;
    std::fstream toWrite(filename, ios::out | ios::binary);

    if (!toWrite) {
        throw ImageSaveException();
    }

    char header[] = {'B', 'M'};
    toWrite.write(header, sizeof(header));
    toWrite.write(reinterpret_cast<char *>(this->bmpFileHeader), sizeof(bmp_file_header));
    toWrite.write(reinterpret_cast<char *>(this->bmpInfoHeader), sizeof(bmp_info_header));

    unsigned padding = 0;
    if (((this->bmpInfoHeader->width * sizeof(RGB)) % 4) != 0) {
        padding = (4 - ((this->bmpInfoHeader->width * sizeof(RGB))) % 4);
    }

    int written = 0;
    for (int row = 0; row < this->bmpInfoHeader->height; row++) {
        for (int column = 0; column < this->bmpInfoHeader->width; column++) {
            RGB pixel = this->getPixelAt(column, row, this->bmpInfoHeader->width);
            toWrite.write(reinterpret_cast<char *>(&pixel), 3);
        }
        if (padding != 0) {
            toWrite.seekg(padding, std::ios_base::cur);
        }
    }

    written = 0;
    for (uint8_t byte: *this->restOfTheFile) {
        toWrite.write(reinterpret_cast<char *>(&byte), 1);
        written++;
    }
}
