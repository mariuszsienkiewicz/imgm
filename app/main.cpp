#include <iostream>
#include <regex>

#include "PgmImage.h"
#include "BmpImage.h"

/**
 * Displays the help message.
 */
void help() {
    using std::cout; using std::endl;

    cout << "This program gives you the possibility to edit images in these two formats: bmp and pgm." << endl << endl;
    cout << "Format: imgm -i <file.bpm|pgm> <flag><value|s?>* -o output.bmp|pgm" << endl << endl;
    cout << "Flags supported:" << endl;
    cout << "\t -i - path to the input image" << endl;
    cout << "\t -o - path where the image should be saved" << endl;
    cout << "\t -rs - resize, expects two values after the flag, width and height separated by space" << endl;
    cout << "\t -n - negative" << endl;
    cout << "\t -b - blur, expects one value after the flag there is one possibility now: 1 - average filter" << endl;
    cout << "\t -dn - reduce noise, expects one value after the flag. There is one possibility now: 1 - median filter" << endl;
    cout << "\t -g - gradient filter, expects one value after the flag. There is one possibility now: 1 - Sobel operator" << endl;
    cout << "\t -ib - to binary image, expects one value after the flag, it's the threshold" << endl;
    cout << "\t -e - erode (binary image (-ib) must be specified before)" << endl;
    cout << "\t -d - dilate (binary image (-ib) must be specified before)" << endl;
    cout << "\t -r - rotate, expects one value after the flag, it's the rotation degree" << endl;
    cout << "\t -h - this help message" << endl;
}

/**
 * Exception that is thrown when filename was not passed via program arguments.
 */
class NoFileNameException : std::exception {
    const char * what () const noexcept override {
        return "Mandatory filename parameter not passed to argument";
    }
};

/**
 * Exception that is thrown when argument was not recognized.
 */
struct WrongArgument : std::exception {
    const char * what() const noexcept override {
        return "This argument is not supported by this program.";
    }
};

/**
 * Exception that is thrown when argument has a parameter that is not supported by this program.
 */
struct UnsupportedTypeParameter : std::exception {
    const char * what() const noexcept override {
        return "Unsupported type method";
    }
};

/**
 * Exception thrown when there is a missing parameter in argument
 */
struct MissingArgumentParameter : std::exception {
    const char * what () const noexcept override {
        return "Missing argument parameter";
    }
};

/**
 * Exception thrown when there is any problem with argument parameter.
 */
struct WrongArgumentParameter : std::exception {
    const char * what () const noexcept override {
        return "Bad argument parameter";
    }
};

/**
 * Enum containing all the possible program arguments
 */
enum Arguments {
    INPUT_FILE,
    OUTPUT,
    RESOLUTION_CHANGE,
    NEGATIVE,
    BLUR,
    DENOISE,
    GRADIENT,
    BINARY,
    ERODE,
    DILATE,
    ROTATE,
    HELP,
    INVALID
};

/**
 * Function that gives the proper Argument by passed string.
 * @param argument - string that will be checked
 * @return - Proper Argument or invalid.
 */
Arguments stringToArgument(const std::string& argument) {
    if (argument == "-i") return INPUT_FILE;
    if (argument == "-o") return OUTPUT;
    if (argument == "-e") return ERODE;
    if (argument == "-rs") return RESOLUTION_CHANGE;
    if (argument == "-n") return NEGATIVE;
    if (argument == "-b") return BLUR;
    if (argument == "-dn") return DENOISE;
    if (argument == "-g") return GRADIENT;
    if (argument == "-ib") return BINARY;
    if (argument == "-e") return ERODE;
    if (argument == "-d") return DILATE;
    if (argument == "-r") return ROTATE;
    if (argument == "-h") return HELP;

    return INVALID;
}

/**
 * Checks if the toValidate is in the fe. -f format.
 * @param toValidate
 * @return bool - true if the parameter is in the correct form, false otherwise.
 */
bool wasParameterPassed(const std::string& toValidate) {
    std::regex flagRegex("-.*");
    if (!toValidate.empty() && !std::regex_match(toValidate, flagRegex)) {
        return true;
    }

    return false;
}

/**
 * Returns the next argument if there is any in the array or empty string.
 * @param argv - array that will be checked.
 * @param actualIndex - next index after this index will be checked.
 * @param size_t - size of the array
 * @return returns next argument or empty if nothing was found.
 */
std::string getNextArg(char * argv[], const int actualIndex, const int size_t) {
    if (actualIndex + 1 < size_t) {
        return argv[actualIndex + 1];
    }

    return "";
}

/**
 * Main function - it is responsible for argument parsing and sending commands to the Image classes.
 * @param argc - number of arguments
 * @param argv - arguments array
 * @return -1 - if there was a problem | 0 - if not | -10 - problem with arguments | -11 problem with the file.
 */
int main(int argc, char *argv[]) {
    using std::cout; using std::endl;

    // if there is no command line arguments passed then display the program manual.
    if (argc < 2) {
        help();
        return -1;
    }

    std::string firstArg = argv[1];
    if (firstArg == "-h") {
        help();
        return 0;
    } else if (firstArg != "-i") {
        cout << "Wrong argument! See the program manual:" << endl;
        help();
        return -1;
    }

    std::regex bmpRegexFile(".*.bmp");
    std::regex pgmRegexFile(".*.pgm");

    Image* image;

    try {
        std::fstream file;
        if (std::regex_match(argv[2], bmpRegexFile)) {
            file = std::fstream(argv[2], std::ios::in | std::ios::binary);
            image = new BmpImage(file);
        } else if (std::regex_match(argv[2], pgmRegexFile)) {
            file = std::fstream(argv[2], std::ios::in | std::ios::binary);
            image = new PgmImage(file);
        } else {
            std::cerr << "Wrong filename format or the file format is not supported by this program." << endl;
            std::cerr << "See the program manual." << endl;
            help();
            return -1;
        }

        image->read();

        std::string arg;
        try {
            for (int argNum = 3; argNum < argc; argNum++) {
                arg = argv[argNum];
                switch (stringToArgument(arg)) {
                    case OUTPUT: {
                        std::string filename = getNextArg(argv, argNum, argc);
                        if (wasParameterPassed(filename)) {
                            image->save(filename);
                        } else {
                            throw NoFileNameException();
                        }
                        argNum++;
                        break;
                    }
                    case RESOLUTION_CHANGE: {
                        int tempArgNum = argNum;
                        std::string x = getNextArg(argv, tempArgNum, argc);
                        tempArgNum++;
                        std::string y = getNextArg(argv, tempArgNum, argc);
                        tempArgNum++;

                        if (!wasParameterPassed(x) || !wasParameterPassed(y)) {
                            throw MissingArgumentParameter();
                        }

                        int xInt, yInt;
                        try {
                            xInt = std::stoi(x);
                            yInt = std::stoi(y);
                        } catch (std::exception &ex) {
                            throw WrongArgumentParameter();
                        }

                        image->scale(xInt, yInt);
                        argNum = tempArgNum;

                        break;
                    }
                    case NEGATIVE:
                        image->toNegative();
                        break;
                    case BLUR: {
                        std::string type = getNextArg(argv, argNum, argc);
                        if (wasParameterPassed(type)) {
                            int typeInt;
                            try {
                                typeInt = std::stoi(type);
                            } catch (std::exception &exception) {
                                throw WrongArgumentParameter();
                            }

                            if (typeInt == 1) {
                                image->blur();
                                argNum++;
                            } else {
                                throw UnsupportedTypeParameter();
                            }
                        } else {
                            throw MissingArgumentParameter();
                        }
                        break;
                    }
                    case DENOISE: {
                        std::string size = getNextArg(argv, argNum, argc);
                        argNum++;
                        image->denoise(std::stoi(size));
                        break;
                    }
                    case GRADIENT: {
                        std::string type = getNextArg(argv, argNum, argc);
                        argNum++;

                        if (!wasParameterPassed(type)) {
                            throw MissingArgumentParameter();
                        }

                        if (std::stoi(type) == 1) {
                            image->edgeFilter();
                        } else {
                            throw UnsupportedTypeParameter();
                        }

                        break;
                    }
                    case BINARY: {
                        std::string threshold = getNextArg(argv, argNum, argc);
                        if (wasParameterPassed(threshold)) {
                            int thresholdInt;
                            try {
                                thresholdInt = std::stoi(threshold);
                            } catch (std::exception &exception) {
                                throw WrongArgumentParameter();
                            }

                            image->toBinary(thresholdInt);
                        } else {
                            throw MissingArgumentParameter();
                        }
                        argNum++;
                        break;
                    }
                    case ERODE:
                        image->erode();
                        break;
                    case DILATE:
                        image->dilate();
                        break;
                    case ROTATE: {
                        std::string degree = getNextArg(argv, argNum, argc);
                        if (wasParameterPassed(degree)) {
                            float rotationDegree;
                            try {
                                rotationDegree = std::stof(degree);
                            } catch (std::exception &e) {
                                throw WrongArgumentParameter();
                            }

                            image->rotate(rotationDegree);
                            argNum++;
                        }

                        break;
                    }
                    case HELP:
                        help();
                        break;
                    case INVALID:
                    default: {
                        throw WrongArgument();
                    }
                }
            }

            std::cout << "Done!";

            return 0;
        } catch (std::exception &exception) {
            std::cerr << "Argument exception: " << arg << std::endl;
            std::cerr << "Description: " << exception.what() << std::endl;
            std::exit(-10);
        }
    } catch (std::exception &exception) {
        std::cerr << "Description: " << exception.what() << std::endl;
        std::exit(-11);
    }

    return -1;
}
