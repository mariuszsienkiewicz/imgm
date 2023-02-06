# Image manipulation

A project that introduced me to C++ and the basics of image manipulation.

![visualization](/assets/example.jpg "visualization")
*BMP image, Sobel operator (before/after)*

## Supported operations:

- resize
- negative
- blur
- noise reduction (median filter)
- gradient filter (Sobel operator)
- binary converter
- erode
- dilate
- rotate

## Building

Clone this repository and then:

```console
foo@bar:~$ mkdir build && cd build
foo@bar:~$ cmake .. -DCMAKE_BUILD_TYPE=[Release]
foo@bar:~$ make
foo@bar:~$ ./imgm
```

## Usage

Gradient:

```console
foo@bar:~$ ./imgm -i ../sample/jet.bmp -g 1 -o test.bmp # result will be the same as in the readme image
```

Help message:

```
Format: imgm -i <file.bpm|pgm> <flag><value|s?>* -o output.bmp|pgm

Flags supported:
    -i - path to the input image
    -o - path where the image should be saved
    -rs - resize, expects two values after the flag, width and height separated by space
    -n - negative
    -b - blur, expects one value after the flag there is one possibility now: 1 - average filter
    -dn - reduce noise, expects one value after the flag. There is one possibility now: 1 - median filter
    -g - gradient filter, expects one value after the flag. There is one possibility now: 1 - Sobel operator
    -ib - to binary image, expects one value after the flag, it's the threshold
    -e - erode (binary image (-ib) must be specified before)
    -d - dilate (binary image (-ib) must be specified before)
    -r - rotate, expects one value after the flag, it's the rotation degree
    -h - help message
```

You need to specify arguments in correct order (input image, operations, output image).