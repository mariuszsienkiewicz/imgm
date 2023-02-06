#ifndef PIXELMANAGER_H
#define PIXELMANAGER_H

/**
 * PixelManager - gives the Image the possibility to easily operate on the 1D array.
 * @tparam PIXEL_TYPE - type of the pixel that the image uses.
 */
template<typename PIXEL_TYPE>
class PixelManager {
private:
    /**
     * Array of pixels.
     */
    PIXEL_TYPE * pixels;

public:
    /**
     * Sets PIXEL_TYPE array to the class member.
     * @param pixelToSet
     */
    void setPixels(PIXEL_TYPE * pixelToSet);

    /**
     * Returns the pixel at the given position
     * @param x - position in the x axis
     * @param y - position in the y axis
     * @param width - width of the whole image (in pixels)
     * @return PIXEL_TYPE - pixel
     */
    PIXEL_TYPE & getPixelAt(int x, int y, int width) const;

    /**
     * Returns the pixel value at the given position from the given array
     * @param x - position in the x axis
     * @param y - position in the y axis
     * @param width - width of the whole image (in pixels)
     * @param array - array where the search will be made
     * @return PIXEL_TYPE - pixel
     */
    PIXEL_TYPE getPixelAt(int x, int y, int width, const PIXEL_TYPE * array) const;

    /**
     * Gives the pixel at specific index.
     * @param index - index where the pixel should be located
     * @return PIXEL_TYPE - pixel
     */
    PIXEL_TYPE & getPixelByIndex(int index) const;

    /**
     * Sets the pixel in the given array - it assumes that memory for it is in place.
     * @param x - position in the x axis
     * @param y - position in the y axis
     * @param width - width of the whole image (in pixels) - used for decoding the real position
     * @param tempPixels - array to which the RGB variable will be inserted
     * @param rgb - pixel to be inserted
     */
    void setPixelAt(int x, int y, int width, PIXEL_TYPE *array, PIXEL_TYPE pixel);
};

template<typename PIXEL_TYPE>
PIXEL_TYPE &PixelManager<PIXEL_TYPE>::getPixelAt(int x, int y, int width) const {
    return this->pixels[x + y * width];
}

template<typename PIXEL_TYPE>
PIXEL_TYPE PixelManager<PIXEL_TYPE>::getPixelAt(int x, int y, int width, const PIXEL_TYPE * array) const {
    return array[x + y * width];
}

template<typename PIXEL_TYPE>
PIXEL_TYPE & PixelManager<PIXEL_TYPE>::getPixelByIndex(int index) const {
    return this->pixels[index];
}

template<typename PIXEL_TYPE>
void PixelManager<PIXEL_TYPE>::setPixelAt(int x, int y, int width, PIXEL_TYPE *array, PIXEL_TYPE pixel) {
    array[x + y * width] = pixel;
}

template<typename PIXEL_TYPE>
void PixelManager<PIXEL_TYPE>::setPixels(PIXEL_TYPE * pixelToSet) {
    this->pixels = pixelToSet;
}

#endif //PIXELMANAGER_H
