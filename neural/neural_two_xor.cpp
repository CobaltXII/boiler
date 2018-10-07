// Neural networks? Awesome.

#include "../boiler/boiler.h"

#include <random>
#include <vector>
#include <utility>
#include <fstream>
#include <iostream>

#include "neural_matrix.h"
#include "neural_network.h"

struct game: boiler
{	
	// Make a neural network.

	neural_network my_brain = neural_network(2, 8, 1, 0.5);

	// Create some training data.

	std::vector<std::pair<std::vector<double>, std::vector<double>>> train_data =
	{
		{
			{0.0, 0.0}, {0.0}
		},
		{
			{0.0, 1.0}, {1.0}
		},
		{
			{1.0, 0.0}, {1.0}
		},
		{
			{1.0, 1.0}, {0.0}
		}
	};

	void steam() override
	{
		width = 800;
		height = 600;

		f_No_Debug = SDL_TRUE;

		title = "Neural network solving XOR (using Boiler)";

		// Load from save, if a previous save exists.

		std::ifstream _In_File = std::ifstream("brain_xor");

		if (_In_File.good())
		{
			my_brain = neural_network::load("brain_xor");
		}
	}

	void keydown(SDL_Event e) override
	{
		if (e.key.keysym.sym == SDLK_SPACE)
		{
			my_brain = neural_network(2, 4, 1, 0.5);
		}
	}

	void draw() override
	{
		// Visualize a two-dimensional function.

		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		double r_x = 40;
		double r_y = 40;

		double pg_x = 520;
		double pg_y = 520;

		double dpr_x = pg_x / r_x;
		double dpr_y = pg_y / r_y;

		int o_x = (int)h_width - (pg_x / 2);
		int o_y = (int)h_height - (pg_y / 2);

		std::vector<double> _In;

		for (int i = 0; i < r_x; i++)
		{
			for (int j = 0; j < r_y; j++)
			{
				double n_x = i / r_x;
				double n_y = j / r_y;

				_In = {n_x, n_y};

				std::vector<double> f = my_brain.feed_forward(_In);

				frectrgb
				(
					o_x + (i * dpr_x), 
					o_y + (j * dpr_y), 

					dpr_x, 
					dpr_y, 

					rgb
					(
						clamprgb(f[0] * 256), 
						clamprgb(f[0] * 256), 
						clamprgb(f[0] * 256)
					)
				);
			}
		}

		// Train the neural network.

		for (int i = 0; i < 1; i++)
		{
			for (int j = 0; j < train_data.size(); j++)
			{
				my_brain.train
				(
					train_data[j].first,
					train_data[j].second
				);
			}
		}

		if (iteration % 10 == 0)
		{
			// Calculate the average error.

			double a_E = 0.0;

			for (int j = 0; j < train_data.size(); j++)
			{
				matrix m_Fdd = matrix(my_brain.feed_forward(train_data[j].first));

				matrix m_Ers = matrix(train_data[j].second);

				matrix m_Edf = matrix::subtract(m_Ers, m_Fdd);

				matrix m_Err = matrix::map(m_Edf, std::abs);

				for (int i = 0; i < m_Err.r_C; i++)
				{
					a_E += m_Err.u_M[i][0];
				}
			}

			std::cout << "Average error (epoch " << (iteration + 1) * 10  << "): " << a_E / (my_brain.o_nodes * train_data.size()) << std::endl;
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