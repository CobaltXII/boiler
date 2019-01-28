#define BOIL_USE_STB_IMAGE

#include "../boiler/boiler.h"

#include <random>
#include <vector>
#include <utility>
#include <iostream>

std::string path_to_img;

// Initialize a good random number generator.

std::default_random_engine rng_source;

// An image is a one-dimensional array of unsigned integers. However, a one-dimensional array of
// unsigned integers can be any number of other things as well. For clarity, we use a typedef.

typedef Uint32* image_rgb;

// Similar to the above statement, a grayscale image can be represented by a one-dimensional array
// of unsigned chars. An array of unsigned chars is ambiguous, so we use a typedef.

typedef Uint8* image_gs;
