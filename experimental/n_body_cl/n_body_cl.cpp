// Emptyness? Awesome.

#include "../../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

// Include OpenCL.

#ifdef __APPLE__

#define CL_SILENCE_DEPRECATION

#include <OpenCL/OpenCL.h>

#else

#include <CL/cl.h>

#endif
