#include "Tools.h"

uint8_t Tools::median(uint8_t array[], const size_t size) {
    if (size == 0) {
        return 0;
    } else {
        std::sort(array, array + size);
        if (size % 2 == 0) {
            return (array[size / 2 - 1] + array[size / 2]) / 2;
        } else {
            return array[size / 2];
        }
    }
}
