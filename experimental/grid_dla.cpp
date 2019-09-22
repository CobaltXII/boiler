#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

// A walker.
struct walker {
	int old_x;
	int old_y;
	int x;
	int y;
	bool active;
};

// Random directions.
int random_dir_x[8] = {-1,  0,  1, -1,  1, -1,  0,  1};
int random_dir_y[8] = {-1, -1, -1,  0,  0,  1,  1,  1};

struct game: boiler {
	// The walkers in the simulation.
	std::vector<walker> walkers;

	void steam() override {
		width = 800;
		height = 800;
		title = "Diffusion-limited aggregation (using Boiler)";

		// Add random walkers.
		for (int i = 0; i < 16384 * 16; i++) {
			int x = rand() % width;
			int y = rand() % height;
			walkers.push_back({x, y, x, y, true});
		}
	}

	void draw() override {
		if (iteration == 0) {
			// Clear the screen.
			black();

			// Fill the center pixel.
			int cx = width / 2;
			int cy = height / 2;
			pixels[cy * width + cx] = rgb(255, 255, 255);
		}

		for (int i = 0; i < walkers.size(); i++) {
			walker& my_walker = walkers[i];

			if (my_walker.active) {
				// Set the walker's old position.
				my_walker.old_x = my_walker.x;
				my_walker.old_y = my_walker.y;

				// Move the walker randomly.
				int random_dir = rand() % 8;
				my_walker.x += random_dir_x[random_dir];
				my_walker.y += random_dir_y[random_dir];

				// Walkers that move out of the grid should be moved to a random
				// position on the grid.
				if (my_walker.x < 0 || my_walker.x >= width ||
					my_walker.y < 0 || my_walker.y >= height) {
					my_walker.active = false;
					continue;
					// my_walker.x = rand() % width;
					// my_walker.y = rand() % height;
				}

				// Check for collisions with filled cells.
				if (pixels[my_walker.y * width + my_walker.x]) {
					// Set the previous position of the walker on the grid to
					// filled.
					pixels[my_walker.old_y * width + my_walker.old_x] = rgb(255, 255, 255);

					my_walker.active = false;
					// Move the walker to a random position.
					// my_walker.x = rand() % width;
					// my_walker.y = rand() % height;
				}
			}
		}
	}
};

int main(int argc, char** argv) {
	game demo;
	if (demo.make() != 0) {
		std::cout << "Could not initialize Boiler." << std::endl;
		return 1;
	}
	demo.engine();
	demo.sweep();
	return 0;
}