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

// Include the kernel source.

#define __stringify(source) #source

const char* kernel_source =

#include "n_body_cl.cl"

#undef __stringify

size_t kernel_source_size = strlen(kernel_source);

#include "../../gradient/gradient.h"

// Write a message to std::cout.

void say(std::string message)
{
	std::cout << message << std::endl;
}

// The Boiler rendering context.

struct game: boiler
{	
	gradient colormap = gradient_from_string("1.000000 0.941177 0.588235 0.039215 0.666666 0.745098 0.392157 0.117647 0.333333 0.627451 0.196078 0.274510 0.000000 0.078431 0.078431 0.352941");

};

// Entry point for the software renderer.

int main(int argc, char** argv)
{
	// Create variables to hold return codes.

	cl_int r_code;

	cl_int r_code1;
	cl_int r_code2;

	// Create identifier objects to hold information about the available
	// platforms and available devices.

	cl_platform_id platform_id = NULL;

    cl_device_id device_id = NULL;

    // Create unsigned integer objects to hold the amount of available
    // platforms and available devices.

    cl_uint num_platforms;

    cl_uint num_devices;

    // Get the first available platform and store the amount of available
    // platforms.

    clGetPlatformIDs(1, &platform_id, &num_platforms);

    // Get the first available device on the first available platform. Store
    // the amount of available devices. This device will be referred to as the
    // 'default device'.

    clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &num_devices);

    // Create an OpenCL context on the default device.

    cl_context context = clCreateContext(0, 1, &device_id, NULL, NULL, &r_code);

    // Make sure the OpenCL context was created successfully.

    if (r_code != CL_SUCCESS)
    {
    	say("Could not create an OpenCL context.");

    	return EXIT_FAILURE;
    }

    // Create an OpenCL command queue.

    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &r_code);

    // Make sure the OpenCL command queue was created successfully.

    if (r_code != CL_SUCCESS)
    {
    	say("Could not create an OpenCL command queue.");

    	return EXIT_FAILURE;
    }
	// Initialize Boiler.

	game demo;

	if (demo.make() != 0)
	{
		std::cout << "Could not initialize Boiler." << std::endl;

		return 1;
	}

	demo.engine();
	demo.sweep();

	return 0;
}