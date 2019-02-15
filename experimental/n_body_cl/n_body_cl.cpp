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

	cl_int n;

	cl_float4* state1;
	cl_float4* state2;

	cl_mem gpu_state1;
	cl_mem gpu_state2;

	cl_command_queue command_queue;

	cl_kernel kernel;

	cl_float inv_scale = 32.0f;

	void steam() override
	{
		width = 800;
		
		height = 800;

		title = "Emptyness (using Boiler)";
	}

	void wheelup(SDL_Event e) override
	{
		inv_scale -= 1.0f;
	}

	void wheeldown(SDL_Event e) override
	{
		inv_scale += 1.0f;
	}

	void keydown(SDL_Event e) override
	{
		SDL_Keycode key = e.key.keysym.sym;

		if (key == SDLK_ESCAPE)
		{
			running = SDL_FALSE;
		}
		else if (key == SDLK_r)
		{
			// Re-initialize the n-body simulation.

			const float xr = 16000.0f;
			const float yr = 16000.0f;

			for (int i = 0; i < n; i++)
			{
				// Generate a random body.

				cl_float ang = rand_01() * 2.0f * M_PI;

				cl_float rad = rand_01();

				cl_float x = (xr * rad) * cos(ang);
				cl_float y = (yr * rad) * sin(ang);

				cl_float vx = cos(ang + degrad(90.0f)) * rad * 64.0f;
				cl_float vy = sin(ang + degrad(90.0f)) * rad * 64.0f;

				// Write the body to the first state.

				state1[i] = {x, y, vx, vy};
			}

			// Copy the new initial data to the GPU.

			clEnqueueWriteBuffer(command_queue, gpu_state1, CL_TRUE, 0, n * sizeof(cl_float4), state1, 0, NULL, NULL);
		}
	}

	void draw() override
	{
		black();

		// Do one iteration of the n-body simulation.

		size_t global_work_size = n;

		size_t local_work_size = 256;

		clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);

		// Read state2 back into local CPU memory.

		clEnqueueReadBuffer(command_queue, gpu_state2, CL_TRUE, 0, n * sizeof(cl_float4), state2, 0, NULL, NULL);

		// Swap buffers.

		std::swap(gpu_state1, gpu_state2);

		clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&gpu_state1);
		clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*)&gpu_state2);
	}
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

    // Allocate CPU memory for the n-body simulation.

    cl_int n = 1024 * 24;

    cl_float4* state1 = (cl_float4*)malloc(n * sizeof(cl_float4));
	cl_float4* state2 = (cl_float4*)malloc(n * sizeof(cl_float4));

	// Make sure both arrays were allocated successfully.

	if (!state1 || !state2)
	{
		say("Could not allocate local CPU memory.");

		return EXIT_FAILURE;
	}

	// Initilize the n-body simulation.

	const float xr = 16000.0f;
	const float yr = 16000.0f;

	for (int i = 0; i < n; i++)
	{
		// Generate a random body.

		cl_float ang = rand_01() * 2.0f * M_PI;

		cl_float rad = rand_01();

		cl_float x = (xr * rad) * cos(ang);
		cl_float y = (yr * rad) * sin(ang);

		cl_float vx = cos(ang + degrad(90.0f)) * rad * 64.0f;
		cl_float vy = sin(ang + degrad(90.0f)) * rad * 64.0f;

		// Write the body to the first state.

		state1[i] = {x, y, vx, vy};
	}

	// Clear the second state.

	memset(state2, 0, n * sizeof(cl_float4));

	// Allocate GPU memory for the n-body simulation.

	cl_mem gpu_state1 = clCreateBuffer(context, CL_MEM_READ_WRITE, n * sizeof(cl_float4), NULL, &r_code1);
	cl_mem gpu_state2 = clCreateBuffer(context, CL_MEM_READ_WRITE, n * sizeof(cl_float4), NULL, &r_code2);

	// Make sure both arrays were allocated successfully.

	if (r_code1 != CL_SUCCESS || r_code2 != CL_SUCCESS)
	{
		say("Could not allocate GPU memory.");

		return EXIT_FAILURE;
	}

	// Copy the contents of the CPU n-body simulation memory to the GPU n-body
	// simulation memory.

	r_code1 = clEnqueueWriteBuffer(command_queue, gpu_state1, CL_TRUE, 0, n * sizeof(cl_float4), state1, 0, NULL, NULL);
	r_code2 = clEnqueueWriteBuffer(command_queue, gpu_state2, CL_TRUE, 0, n * sizeof(cl_float4), state2, 0, NULL, NULL);

	// Make sure both arrays were copied successfully.

	if (r_code1 != CL_SUCCESS || r_code2 != CL_SUCCESS)
	{
		say("Could not copy CPU memory to GPU memory.");

		return EXIT_FAILURE;
	}

	// Create an OpenCL program from the kernel source.

	cl_program program = clCreateProgramWithSource(context, 1, (const char**)&kernel_source, (const size_t*)&kernel_source_size, &r_code);

	// Make sure the OpenCL program was created successfully.

	if (r_code != CL_SUCCESS)
	{
		say("Could not create an OpenCL program.");

		return EXIT_FAILURE;
	}

	// Build the OpenCL program.

	r_code = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	// Make sure the OpenCL program was built successfully.

	if (r_code != CL_SUCCESS)
	{
		say("Could not build an OpenCL program.");

		return EXIT_FAILURE;
	}

	// Create the OpenCL kernel from the function "n_body_cl" within the
	// OpenCL program.

	cl_kernel kernel = clCreateKernel(program, "n_body_cl", &r_code);

	// Make sure the OpenCL kernel was created successfully.

	if (r_code != CL_SUCCESS)
	{
		say("Could not create an OpenCL kernel.");

		return EXIT_FAILURE;
	}

	// Set the timestep and softening parameters of the kernel.

	cl_float gpu_float_args[2] = {1200.0f, 50000.0f};

	clSetKernelArg(kernel, 0, sizeof(cl_float), &(gpu_float_args[0]));
	clSetKernelArg(kernel, 1, sizeof(cl_float), &(gpu_float_args[1]));

	// Set the n-body count paramater of the kernel.

	clSetKernelArg(kernel, 2, sizeof(cl_int), &n);

	// Set the state parameters of the kernel.

	clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&gpu_state1);
	clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*)&gpu_state2);

	// Initialize Boiler.

	game demo;

	demo.n = n;

	demo.state1 = state1;
	demo.state2 = state2;

	demo.gpu_state1 = gpu_state1;
	demo.gpu_state2 = gpu_state2;

	demo.command_queue = command_queue;

	demo.kernel = kernel;

	if (demo.make() != 0)
	{
		std::cout << "Could not initialize Boiler." << std::endl;

		return 1;
	}

	demo.engine();
	demo.sweep();

	// Free the kernel.

	clReleaseKernel(kernel);

	// Free the program.

	clReleaseProgram(program);

	// Free the GPU memory.

	clReleaseMemObject(gpu_state1);
	clReleaseMemObject(gpu_state2);

	// Free the command queue.

	clReleaseCommandQueue(command_queue);

	// Free the context.

	clReleaseContext(context);

	// Free the CPU memory.

	free(state1);
	free(state2);

	return 0;
}