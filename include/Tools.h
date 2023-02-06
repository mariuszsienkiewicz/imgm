#ifndef TOOLS_H
#define TOOLS_H

#include <algorithm>
#include <vector>
#include <iostream>

#define PI 3.14159265358979323846

/**
 * Class containing any additional algorithms that are needed for the image processing.
 */
class Tools {
public:
    /**
     * Returns the median value from the array
     * @param array - array that will be used
     * @param size - size of the array
     * @return
     */
    static uint8_t median(uint8_t array[], size_t size);
};

#endif //TOOLS_H
