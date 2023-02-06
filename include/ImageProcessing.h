#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

/**
 * Class containing virtual methods responsible for image manipulation
 * that needs to be overridden by every class with new file format.
 * You shouldn't use this class directly, see the Image.h file.
 */
class ImageProcessing {
public:
    /**
     * Blurs out the image.
    */
    virtual void blur() = 0;

    /**
     * Changes the image to the binary format - you should use this went you want to erode or dilate the image
     * @param threshold int threshold that will be used in processing
     */
    virtual void toBinary(int threshold) = 0;

    /**
     * Erodes the image - it needs to be in the binary format first.
     */
    virtual void erode() = 0;

    /**
     * Dilates the image - it needs to be in the binary format first.
     */
    virtual void dilate() = 0;

    /**
     * Changes the image colours to negative.
     */
    virtual void toNegative() = 0;

    /**
     * Takes the width and height dimensions (in pixels) - and performs the scale up or down algorithms on the image.
     * @param newWidth
     * @param newHeight
     */
    virtual void scale(int newWidth, int newHeight) = 0;

    /**
     * Scales up the image - you can use it directly or use the scale function.
     * @param newWidth
     * @param newHeight
     */
    virtual void scaleUp(int newWidth, int newHeight) = 0;

    /**
     * Scales down the image - you can use it directly or use the scale function.
     * @param newWidth
     * @param newHeight
     */
    virtual void scaleDown(int newWidth, int newHeight) = 0;

    /**
     * Applies the edge filter to the image.
     */
    virtual void edgeFilter() = 0;

    /**
     * Reduces noise in the image.
     * @param size - more specific - radius of the window which will be used to reduce noise in the image.
     */
    virtual void denoise(int size) = 0;

    /**
     * Rotates the image
     * @param degree - the degree by which the image will be rotated
     */
    virtual void rotate(float degree) = 0;

    /**
     * Exception thrown if erode or dilate is executed but image data is not in binary format.
     */
    struct NotInBinaryFormatException : std::exception {
        const char * what() const noexcept override {
            return "The image data needs to be in binary format.";
        }
    };
};

#endif //IMAGEPROCESSING_H
