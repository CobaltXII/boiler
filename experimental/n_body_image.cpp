// Emptyness? Awesome.

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../boiler/stb/stb_image_write.hpp"

#include <vector>
#include <utility>
#include <sstream>
#include <iostream>

typedef float real;

inline real degrad(real x)
{
	return 2.0f * M_PI * (x / 360.0f);
}

struct body
{
	real x;
	real y;

	real vx;
	real vy;

	body(real _x, real _y, real _vx = 0.0f, real _vy = 0.0f)
	{
		x = _x;
		y = _y;

		vx = _vx;
		vy = _vy;
	}
};

inline void body_force(std::vector<body>& bodies, real timestep)
{
	const real softening = 1024.0f * 8.0f;

	#pragma omp parallel for schedule(dynamic)

	for (int i = 0; i < bodies.size(); i++)
	{
		real fx = 0.0f;
		real fy = 0.0f;

		for (int j = 0; j < bodies.size(); j++)
		{
			if (i == j)
			{
				continue;
			}

			real dx = bodies[j].x - bodies[i].x;
			real dy = bodies[j].y - bodies[i].y;

			real dist_sqr = dx * dx + dy * dy;

			if (dist_sqr < softening)
			{
				dist_sqr = softening;
			}

			real inv_dist = 1.0f / sqrtf(dist_sqr);

			real inv_dist_3 = 
			(
				inv_dist *
				inv_dist *
				inv_dist
			);

			fx += dx * inv_dist_3;
			fy += dy * inv_dist_3;
		}

		bodies[i].vx += timestep * fx;
		bodies[i].vy += timestep * fy;
	}
}

// Entry point for the software renderer.

int main(int argc, char** argv)
{
	std::vector<body> bodies;

	int x_res = 1024;
	int y_res = 1024;

	unsigned char* img_data = (unsigned char*)malloc(x_res * y_res * 3 * sizeof(unsigned char));

	int stride_in_bytes = x_res * 3;

	const real xr = 6000.0f;
	const real yr = 6000.0f;

	srand(1419);

	for (int i = 0; i < 50000; i++)
	{
		real r1 = real(rand()) / real(RAND_MAX) * 2.0f - 1.0f;
		real r2 = real(rand()) / real(RAND_MAX) * 2.0f - 1.0f;
		
		real angx = cos(r1 * 2.0f * M_PI);
		real angy = sin(r1 * 2.0f * M_PI);

		real x = angx * r2 * xr;
		real y = angy * r2 * yr;

		real vx = cos(r1 * 2.0f * M_PI + degrad(135.0f)) * r2 * 32.0f;
		real vy = sin(r1 * 2.0f * M_PI + degrad(135.0f)) * r2 * 32.0f;

		bodies.push_back(body(x, y, vx, vy));
	}

	for (int i = 0; i < 3600; i++)
	{
		memset(img_data, 0, x_res * y_res * 3 * sizeof(unsigned char));

		body_force(bodies, 250.0f);

		for (int i = 0; i < bodies.size(); i++)
		{
			body& bi = bodies[i];

			bi.x += bi.vx;
			bi.y += bi.vy;

			int x = bi.x / 16.0f + x_res / 2;
			int y = bi.y / 16.0f + y_res / 2;

			if (x >= 0 && x < x_res && y >= 0 && y < y_res)
			{
				img_data[y * stride_in_bytes + x * 3 + 0] = std::min(255, int(img_data[y * stride_in_bytes + x * 3 + 0]) + 64);
				img_data[y * stride_in_bytes + x * 3 + 1] = std::min(255, int(img_data[y * stride_in_bytes + x * 3 + 1]) + 64);
				img_data[y * stride_in_bytes + x * 3 + 2] = std::min(255, int(img_data[y * stride_in_bytes + x * 3 + 2]) + 64);
			}
		}

		std::stringstream path_builder;

		path_builder << "./n_body_image/frame_" << i << ".png";

		stbi_write_png(path_builder.str().c_str(), x_res, y_res, 3, img_data, stride_in_bytes);

		std::cout << i << std::endl;
	}

	free(img_data);

	return 0;
}