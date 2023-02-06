#ifndef IMAGEREADER_H
#define IMAGEREADER_H

/**
 * Basic methods that need to be implemented to make image format classes compatible with the rest of the code.
 * Reading is different for every format so there will be no forced methods.
 */
class ImageReader {
private:

    /**
     * Filestream of the file image data.
     */
    std::fstream *fileStream;

    /**
     * Checks the signature of the image
     * @param fileStream - fileStream - it will be used to read all the necessary data.
     * @return bool - true if the signature is valid, false otherwise.
     */
    virtual bool checkSignature() = 0;
public:

    /**
    * Read the Image data.
    */
    virtual void read() = 0;

    /**
     * Saves the image to the path that is passed via parameter.
     * @param path - path to the new image
     */
    virtual void save(std::string path) const = 0;

    /**
     * Sets the file stream to the class member
     * @param streamToSet - fstream to set
     */
    void setFileStream(std::fstream & streamToSet) {
        this->fileStream = &streamToSet;
    }

    /**
     * Returns the fstream to the input image file data.
     * @return
     */
    std::fstream * getFileStream() {
        return this->fileStream;
    }

    /**
     * Closes the file stream.
     */
    void closeFileStream() {
        this->fileStream->close();
    }

    /**
     * Exception thrown when signature is not correct.
     */
    struct WrongFileFormatException : std::exception {
        const char * what() const noexcept override {
            return "Signature in the file is not correct - file format is not supported.";
        }
    };

    /**
     * Exception thrown when the was any problem with the file.
     */
    struct OpeningTheFileException : std::exception {
        const char * what() const noexcept override {
            return "There has been an error while opening the file.";
        }
    };

    /**
     * Exception thrown if the image has headers or different other things not supported.
     */
    struct WrongMetadataException : std::exception {
        const char * what() const noexcept override {
            return "The image data needs to be in binary format.";
        }
    };

    /**
     * Exception thrown in every case when the image couldn't be saved.
     */
    struct ImageSaveException : std::exception {
        const char * what() const noexcept override {
            return "The image couldn't be saved in the desired location.";
        }
    };
};

#endif //IMAGEREADER_H
