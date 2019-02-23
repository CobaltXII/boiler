// Verlet sandbox? Awesome.

#define BOIL_USE_OPENCL

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

#include "jet.h"

const char* kernel_source =

#include "verlet_fluid_cl.cl"

size_t kernel_source_size = strlen(kernel_source);

struct game: boiler
{
	const float dt = 0.25f;

	const float bounds = 4000.0f;

	const float r = 40.0f;

	const float viscosity = 1.0f;

	boiler_opencl opencl = load_opencl();

	cl_int n = 1024 * 4;

	cl_mem gpu_state0;
	cl_mem gpu_state1;
	cl_mem gpu_state2;

	cl_float3* cpu_state0;
	cl_float3* cpu_state1;

	cl_kernel kernel;

	// Scaling.

	float scale = 1.0f / powf(1.1f, 23.0f);

	void wheelup(SDL_Event e) override
	{
		scale *= 1.1f;
	}

	void wheeldown(SDL_Event e) override
	{
		scale /= 1.1f;
	}

	// Initialize Boiler.

	void steam() override
	{
		width = 960;
		
		height = 960;

		title = "Verlet sandbox (using Boiler)";

		// Unpack OpenCL driver.

		cl_context context = opencl.context;

		cl_command_queue command_queue = opencl.command_queue;

		// Allocate space for the points, CPU side.

		cpu_state0 = (cl_float3*)malloc(n * sizeof(cl_float3));
		cpu_state1 = (cl_float3*)malloc(n * sizeof(cl_float3));

		if (!cpu_state0 || !cpu_state1)
		{
			say("Could not allocate CPU memory.");

			exit(EXIT_FAILURE);
		}

		// Allocate space for the points, GPU side.

		cl_int r_code0;
		cl_int r_code1;
		cl_int r_code2;

		gpu_state0 = clCreateBuffer(context, CL_MEM_READ_WRITE, n * sizeof(cl_float3), NULL, &r_code0);
		gpu_state1 = clCreateBuffer(context, CL_MEM_READ_WRITE, n * sizeof(cl_float3), NULL, &r_code1);
		gpu_state2 = clCreateBuffer(context, CL_MEM_READ_WRITE, n * sizeof(cl_float3), NULL, &r_code2);

		if (r_code0 != CL_SUCCESS || r_code1 != CL_SUCCESS || r_code2 != CL_SUCCESS)
		{
			say("Could not allocate GPU memory.");

			exit(EXIT_FAILURE);
		}

		// Initialize CPU memory.

		memset(cpu_state0, 0, n * sizeof(cl_float3));
		memset(cpu_state1, 0, n * sizeof(cl_float3));

		for (int i = 0; i < n; i++)
		{
			// Generate a random body.

			cl_float ang = rand_01() * 2.0f * M_PI;

			cl_float rad = rand_01();

			if (true)
			{
				// Uniform distribution.

				rad = sqrtf(rad);
			}

			cl_float x = (bounds * rad) * cos(ang);
			cl_float y = (bounds * rad) * sin(ang);

			cl_float vx = cos(ang + degrad(90.0f)) * rad * 16.0f;
			cl_float vy = sin(ang + degrad(90.0f)) * rad * 16.0f;

			cpu_state0[i] = {x - vx, y - vy, 0.0f};

			cpu_state1[i] = {x, y, 0.0f};
		}

		// Copy the CPU memory to the GPU memory.

		r_code0 = clEnqueueWriteBuffer(command_queue, gpu_state0, CL_TRUE, 0, n * sizeof(cl_float3), cpu_state0, 0, NULL, NULL);
		r_code1 = clEnqueueWriteBuffer(command_queue, gpu_state1, CL_TRUE, 0, n * sizeof(cl_float3), cpu_state1, 0, NULL, NULL);

		if (r_code0 != CL_SUCCESS || r_code1 != CL_SUCCESS)
		{
			say("Could not copy CPU memory to GPU memory.");

			exit(EXIT_FAILURE);
		}

		// Load the required program.

		cl_program program = opencl.load_program(kernel_source, kernel_source_size);

		// Load the required kernel.

		kernel = opencl.load_kernel(program, "verlet_fluid");

		// Set up required arguments.

		cl_float args[4] = {dt, bounds, r, viscosity};

		clSetKernelArg(kernel, 0, sizeof(cl_float), &(args[0]));
		clSetKernelArg(kernel, 1, sizeof(cl_float), &(args[1]));
		clSetKernelArg(kernel, 2, sizeof(cl_float), &(args[2]));
		clSetKernelArg(kernel, 3, sizeof(cl_float), &(args[3]));

		clSetKernelArg(kernel, 4, sizeof(cl_int), &n);

		clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&gpu_state0);
		clSetKernelArg(kernel, 6, sizeof(cl_mem), (void*)&gpu_state1);
		clSetKernelArg(kernel, 7, sizeof(cl_mem), (void*)&gpu_state2);
	}

	// Handle a key press using Boiler.

	void keydown(SDL_Event e) override
	{
		SDL_Keycode key = e.key.keysym.sym;

		if (key == SDLK_ESCAPE)
		{
			running = SDL_FALSE;
		}
	}

	// Draw a frame using Boiler.

	void draw() override
	{
		black();

		// Unpack OpenCL driver.

		cl_context context = opencl.context;

		cl_command_queue command_queue = opencl.command_queue;

		// Send mouse parameters.

		cl_float cl_mouse_x = (mouse_x - h_width) / scale;
		cl_float cl_mouse_y = (mouse_y - h_width) / scale;

		cl_int cl_mouse_down = mouse_l;

		clSetKernelArg(kernel, 8, sizeof(cl_float), &cl_mouse_x);
		clSetKernelArg(kernel, 9, sizeof(cl_float), &cl_mouse_y);

		clSetKernelArg(kernel, 10, sizeof(cl_int), &cl_mouse_down);

		// Do one iteration.

		size_t global_work_size = n;

		size_t local_work_size = 256;

		for (int i = 0; i < 4; i++)
		{
			clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);

			// Swap buffers.

			std::swap(gpu_state1, gpu_state2);

			clSetKernelArg(kernel, 6, sizeof(cl_mem), (void*)&gpu_state1);
			clSetKernelArg(kernel, 7, sizeof(cl_mem), (void*)&gpu_state2);
		}

		// Read state2 back into local CPU memory.

		clEnqueueReadBuffer(command_queue, gpu_state2, CL_TRUE, 0, n * sizeof(cl_float3), cpu_state1, 0, NULL, NULL);

		// Render the points.

		for (int i = 0; i < n; i++)
		{
			float x = cpu_state1[i].s[0] * scale + h_width;
			float y = cpu_state1[i].s[1] * scale + h_width;

			if (r * scale > 1.0f)
			{
				circlergb(x, y, r * scale + 1, 0xFFFFFF);
			}
			else
			{
				plotp(x, y, jet_colormap[int(mclamprgb(cpu_state1[i].s[2]))]);
			}
		}
	}
};

// Entry point for the software renderer.

int main(int argc, char** argv)
{
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