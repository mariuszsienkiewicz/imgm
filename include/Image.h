#ifndef IMAGE_H
#define IMAGE_H

#include "ImageReader.h"
#include "ImageProcessing.h"

/**
 * This class should be inherited to be properly used in the program.
 * It will pass you all the necessary virtual methods that you should implement in the new image format.
 */
class Image : public ImageProcessing, public ImageReader {
    /**
     * Validate the Image.
     */
    virtual void validate() = 0;
};

#endif //IMAGE_H
