#include "PgmImage.h"
#include "Tools.h"

// ImageReader methods implementation and additional necessary methods

int vectorOfCharsToInt(const std::vector<char>& partsOfNumber) {
    std::string stringResult;
    for (char sign : partsOfNumber) {
        stringResult += sign;
    }

    return std::stoi(stringResult);
}

PgmImage::PgmImage(std::fstream & fileStream) : PixelManager() {
    this->setFileStream(fileStream);
}

void PgmImage::read() {
    this->validate();

    skipNextByte();
    readInfoHeader();
    readPixels();

    this->closeFileStream();
}

void PgmImage::validate() {
    if (this->getFileStream()->fail()) {
        throw OpeningTheFileException();
    }


    if (!this->checkSignature()) {
        throw WrongFileFormatException();
    }
}

bool PgmImage::checkSignature() {
    char header[] = {'P', '5'};
    char actualHeader[2];
    this->getFileStream()->read(actualHeader, 2);

    return std::equal(header, header + 2, actualHeader);
}

void PgmImage::readInfoHeader() {
    std::vector<char> widthChunk = readToTheNextWhitespace();
    this->width = vectorOfCharsToInt(widthChunk);

    std::vector<char> heightChunk = readToTheNextWhitespace();
    this->height = vectorOfCharsToInt(heightChunk);

    std::vector<char> maxValChunk = readToTheNextWhitespace();
    this->maxVal = vectorOfCharsToInt(maxValChunk);
}

void PgmImage::skipNextByte() {
    this->getFileStream()->get();
}

std::vector<char> PgmImage::readToTheNextWhitespace() {
    std::vector<char> chunkOfChars;

    char actual;
    while (std::find(this->whitespaces->begin(), this->whitespaces->end(), actual = this->getFileStream()->get()) == this->whitespaces->end()) {
        chunkOfChars.push_back(actual);
    }

    return chunkOfChars;
}

void PgmImage::readPixels() {
    auto * pixelsArray = new uint8_t[this->height * this->width];

    int index = 0;
    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            uint8_t pixel = this->getFileStream()->get();
            pixelsArray[index] = pixel;
            index++;
        }
    }

    this->setPixels(pixelsArray);
}

void PgmImage::save(std::string filename) const {
    using std::ios;
    std::fstream toWrite(filename, ios::out | ios::binary);

    if (!toWrite) {
        throw ImageSaveException();
    }

    char header[] = {'P', '5'};
    char whitespace = 32;
    toWrite.write(header, sizeof(header));
    toWrite.write(&whitespace, sizeof(uint8_t));

    std::string tmp = std::to_string(this->width);
    char * widthChars = const_cast<char *>(tmp.c_str());

    for (int i = 0; i < tmp.length(); i++) {
        toWrite.write(&widthChars[i], 1);
    }
    toWrite.write(&whitespace, sizeof(uint8_t));

    tmp = std::to_string(this->height);
    widthChars = const_cast<char *>(tmp.c_str());
    for (int i = 0; i < tmp.length(); i++) {
        toWrite.write(&widthChars[i], 1);
    }
    toWrite.write(&whitespace, sizeof(uint8_t));

    tmp = std::to_string(this->maxVal);
    widthChars = const_cast<char *>(tmp.c_str());
    for (int i = 0; i < tmp.length(); i++) {
        toWrite.write(&widthChars[i], 1);
    }
    toWrite.write(&whitespace, sizeof(uint8_t));

    int index = 0;
    for (int y = 0; y < this->width; y++) {
        for (int x = 0; x < this->height; x++) {
            toWrite.write(reinterpret_cast<char *>(&this->getPixelByIndex(index)), 1);
            index++;
        }
    }
}

// ImageProcessing methods implementation

void PgmImage::blur() {
    auto * modifiedImg = new uint8_t[this->height * this->width];
    int index = 0;
    for (int row = 0; row < this->height; row++) {
        for (int column = 0; column < this->width; column++) {
            if (row == 0 || column == 0 || column == this->width - 1 ||
                row == this->height - 1) {
                modifiedImg[index] = this->getPixelAt(column, row, this->width);
            } else {
                uint8_t a = this->getPixelAt(column - 1, row - 1, this->width);
                uint8_t b = this->getPixelAt(column - 1, row, this->width);
                uint8_t c = this->getPixelAt(column - 1, row + 1, this->width);

                uint8_t d = this->getPixelAt(column, row - 1, this->width);
                uint8_t x = this->getPixelAt(column, row, this->width);
                uint8_t e = this->getPixelAt(column, row + 1, this->width);

                uint8_t f = this->getPixelAt(column + 1, row - 1, this->width);
                uint8_t g = this->getPixelAt(column + 1, row, this->width);
                uint8_t h = this->getPixelAt(column - 1, row + 1, this->width);

                uint8_t avg = (a + b + c + d + x + e + f + g + h) / 9;

                modifiedImg[index] = avg;
            }

            index++;
        }
    }

    this->setPixels(modifiedImg);
}

void PgmImage::toBinary(int threshold) {
    auto * modifiedImg = new uint8_t[this->height * this->width];
    int index = 0;
    for (int row = 0; row < this->height; row++) {
        for (int column = 0; column < this->width; column++) {
            uint8_t pixel = this->getPixelAt(column, row, this->width);
            uint8_t newPixel;
            if (pixel <= threshold) {
                newPixel = uint8_t(0);
            } else {
                newPixel = this->maxVal;
            }

            modifiedImg[index] = newPixel;
            index++;
        }
    }
    this->setPixels(modifiedImg);
    this->isBinary = true;
}

void PgmImage::erode() {
    if (!this->isBinary) {
        throw NotInBinaryFormatException();
    }

    int radius = 3;

    // create binary image
    auto * modifiedImg = new uint8_t[this->height * this->width];
    int index = 0;
    for (int row = 0; row < this->height; row++) {
        int topEdge = std::min(row + radius, this->height - 1);
        int bottomEdge = std::max(row - radius, 0);

        for (int column = 0; column < this->width; column++) {
            uint8_t actualPixel = this->getPixelAt(column, row, this->width);
            int leftEdge = std::max(column - radius, 0);
            int rightEdge = std::min(column + radius, this->width - 1);
            bool toErode = false;

            for (int y = bottomEdge; y <= topEdge; y++) {
                for (int x = leftEdge; x <= rightEdge; x++) {
                    uint8_t pixel = this->getPixelAt(x, y, this->width);
                    if (pixel == 0) {
                        toErode = true;
                    }
                }
            }

            if (toErode) {
                actualPixel = 0;
            }

            modifiedImg[index] = actualPixel;
            index++;
        }
    }

    this->setPixels(modifiedImg);
}

void PgmImage::dilate() {
    if (!isBinary) {
        throw NotInBinaryFormatException();
    }

    int radius = 2;

    // create binary image
    auto * modifiedImg = new uint8_t[this->height * this->width];
    int index = 0;
    for (int row = 0; row < this->height; row++) {
        int topEdge = std::min(row + radius, this->height - 1);
        int bottomEdge = std::max(row - radius, 0);

        for (int column = 0; column < this->width; column++) {
            uint8_t actualPixel = this->getPixelAt(column, row, this->width);
            int leftEdge = std::max(column - radius, 0);
            int rightEdge = std::min(column + radius, this->width - 1);
            bool toErode = false;

            for (int y = bottomEdge; y <= topEdge; y++) {
                for (int x = leftEdge; x <= rightEdge; x++) {
                    uint8_t pixel = this->getPixelAt(x, y, this->width);
                    if (pixel == maxVal) {
                        toErode = true;
                    }
                }
            }

            if (toErode) {
                actualPixel = maxVal;
            }

            modifiedImg[index] = actualPixel;
            index++;
        }
    }

    this->setPixels(modifiedImg);
}

void PgmImage::toNegative() {
    auto * modifiedImg = new uint8_t[this->height * this->width];
    int index = 0;
    for (int row = 0; row < this->height; row++) {
        for (int column = 0; column < this->width; column++) {
            uint8_t pixel = this->getPixelAt(column, row, this->width);
            uint8_t modified = maxVal - pixel;

            modifiedImg[index] = modified;
            index++;
        }
    }
    this->setPixels(modifiedImg);
}

void PgmImage::scale(int newWidth, int newHeight) {
    if (newWidth > this->width || newHeight > this->height) {
        scaleUp(newWidth, newHeight);
    } else {
        scaleDown(newWidth, newHeight);
    }
}

void PgmImage::scaleUp(int newWidth, int newHeight) {
    double scaleWidth = (double) this->width / newWidth;
    double scaleHeight = (double) this->height / newHeight;

    auto * modifiedImg = new uint8_t[newWidth * newHeight];
    int index = 0;
    for (int row = 0; row < newHeight; row++) {
        for (int column = 0; column < newWidth; column++) {
            uint8_t pixel = this->getPixelAt(column * scaleWidth, row * scaleHeight, this->width);
            modifiedImg[index] = pixel;
            index++;
        }
    }

    this->setPixels(modifiedImg);

    this->width = newWidth;
    this->height = newHeight;
}

void PgmImage::scaleDown(int newWidth, int newHeight) {
    double scaleWidth = (double) newWidth / (double) this->width;
    double scaleHeight = (double) newHeight / (double) this->height;
    int boxWidth = (int) std::ceil(1 / scaleWidth);
    int boxHeight = (int) std::ceil(1 / scaleHeight);

    auto * modifiedImg = new uint8_t[newWidth * newHeight];
    int index = 0;
    for (int row = 0; row < newHeight; row++) {
        for (int column = 0; column < newWidth; column++) {
            int xStartOriginal = std::floor(column / scaleWidth);
            int yStartOriginal = std::floor(row / scaleHeight);
            int xStopOriginal = std::min(xStartOriginal + boxWidth, this->width - 1);
            int yStopOriginal = std::min(yStartOriginal + boxHeight, this->height - 1);

            int sum = 0;
            int count = 0;
            for (int y = yStartOriginal; y <= yStopOriginal; y++) {
                for (int x = xStartOriginal; x <= xStopOriginal; x++) {
                    uint8_t pixel = this->getPixelAt(x, y, this->width);
                    sum += pixel;
                    count++;
                }
            }
            uint8_t calculated = sum / count;

            uint8_t newPixel = calculated;
            modifiedImg[index] = newPixel;
            index++;
        }
    }

    this->setPixels(modifiedImg);

    this->width = newWidth;
    this->height = newHeight;
}

void PgmImage::edgeFilter() {
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

    auto * modifiedImg = new uint8_t[this->width * this->height];
    int index = 0;
    for (int row = 0; row < this->height; row++) {
        int topEdge = std::min(row + kernelSize, this->height - 1);
        int bottomEdge = std::max(row - kernelSize, 0);

        for (int column = 0; column < this->width; column++) {
            int accumulatorX = 0;
            int accumulatorY = 0;

            int leftEdge = std::max(column - kernelSize, 0);
            int rightEdge = std::min(column + kernelSize, this->width - 1);

            uint8_t matrixRGBs[(topEdge - bottomEdge + 1) * (rightEdge - leftEdge + 1)];
            int indexMatrixRGBs = 0;
            for (int y = bottomEdge; y <= topEdge; y++) {
                std::vector<uint8_t> rowVector;
                for (int x = leftEdge; x <= rightEdge; x++) {
                    matrixRGBs[indexMatrixRGBs] = this->getPixelAt(x, y, this->width);
                    indexMatrixRGBs++;
                }
            }

            for (int gY = 0; gY < kernelSize; gY++) {
                for (int gX = 0; gX < kernelSize; gX++) {
                    uint8_t pixel = this->getPixelAt(gX, gY, kernelSize - 1, matrixRGBs);
                    int resultX = pixel * Gx[gY][gX];
                    accumulatorX += resultX;
                    int resultY = pixel * Gy[gY][gX];
                    accumulatorY += resultY;
                }
            }

            int accumulator = sqrt((accumulatorX * accumulatorX) + (accumulatorY * accumulatorY));
            uint8_t resultValue = std::min(accumulator, (int) maxVal);

            modifiedImg[index] = resultValue;
            index++;
        }
    }

    this->setPixels(modifiedImg);
}

void PgmImage::denoise(int size) {
    auto * modifiedImg = new uint8_t[this->width * this->height];
    int radius = (size - 1) / 2;

    int index = 0;
    for (int row = 0; row < this->height; row++) {
        std::vector<uint8_t> modifiedRowVector;
        int topEdge = std::min(row + radius, this->height - 1);
        int bottomEdge = std::max(row - radius, 0);

        for (int column = 0; column < this->width; column++) {
            int leftEdge = std::max(column - radius, 0);
            int rightEdge = std::min(column + radius, this->width - 1);

            size_t arrayOfValuesSize = (topEdge - bottomEdge + 1) * (rightEdge - leftEdge + 1);
            uint8_t values[arrayOfValuesSize];
            int matrixIndex = 0;
            for (int y = bottomEdge; y <= topEdge; y++) {
                for (int x = leftEdge; x <= rightEdge; x++) {
                    uint8_t pixel = this->getPixelAt(x, y, this->width);
                    values[matrixIndex] = pixel;
                    matrixIndex++;
                }
            }

            uint8_t medianValue = Tools::median(values, arrayOfValuesSize);

            modifiedImg[index] = medianValue;
            index++;
        }
    }

    this->setPixels(modifiedImg);
}

void PgmImage::rotate(float degree) {
    auto * modifiedImg = new uint8_t[this->width * this->height];

    int index = 0;
    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            modifiedImg[index] = 0;
            index++;
        }
    }

    double xCenter = this->width / 2.0;
    double yCenter = this->height / 2.0;
    double cos = std::cos(degree * (PI/180.0));
    double sin = std::sin(degree  * (PI/180.0));

    for (int row = 0; row < this->height; row++) {
        for (int column = 0; column < this->width; column++) {
            uint8_t pixel = this->getPixelAt(column, row, this->width);

            double xOffset = column - xCenter;
            double yOffset = row - yCenter;
            int newPosX = (int) (xOffset * cos + yOffset * sin + xCenter);
            int newPosY = (int) (yOffset * cos - xOffset * sin + yCenter);

            if ((newPosX >= 0) && (newPosX < this->width) &&
                (newPosY >= 0) && (newPosY < this->height)) {
                this->setPixelAt(newPosX, newPosY, this->width, modifiedImg, pixel);
            }
        }
    }
    this->setPixels(modifiedImg);
}


