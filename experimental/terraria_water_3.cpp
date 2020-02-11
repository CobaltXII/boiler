// Emptyness? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

const int xr = 100;
const int yr = 100;

const int AIR = 0;
const int SOLID = 1;
const int LIQUID = 2;

int* tiles = new int[xr * yr];
float* masses = new float[xr * yr];

int getTile(int x, int y) {
	if (x < 0 || y < 0 || x >= xr || y >= yr) {
		return 0;
	}
	return tiles[y * xr + x];
}

float getMass(int x, int y) {
	if (x < 0 || y < 0 || x >= xr || y >= yr) {
		return 0;
	}
	return masses[y * xr + x];
}

void setTile(int x, int y, int v) {
	if (x < 0 || y < 0 || x >= xr || y >= yr) {
		return;
	}
	tiles[y * xr + x] = v;
}

void setMass(int x, int y, float v) {
	if (x < 0 || y < 0 || x >= xr || y >= yr) {
		return;
	}
	masses[y * xr + x] = v;
}

struct game: boiler
{	
	void steam() override
	{
		width = 8 * xr;
		height = 8 * yr;

		title = "Emptyness (using Boiler)";

		for (int p = 0; p < xr; p++) {
			setTile(p, 0, SOLID);
			setTile(p, yr - 1, SOLID);
		}
		for (int p = 0; p < yr; p++) {
			setTile(0, p, SOLID);
			setTile(xr - 1, p, SOLID);
		}
	}

	void draw() override
	{
		clear(rgb(255, 255, 255));

		if (mouse_l) {
			int x = mouse_x / 8;
			int y = mouse_y / 8;
			int radius = 4;
			for (int v = -radius; v <= radius; v++) {
				for (int u = -radius; u <= radius; u++) {
					if (u * u + v * v <= radius * radius) {
						setTile(x + u, y + v, LIQUID);
						setMass(x + u, y + v, 1.0f);
					}
				}
			}
		} else if (mouse_r) {
			int x = mouse_x / 8;
			int y = mouse_y / 8;
			int radius = 4;
			for (int v = -radius; v <= radius; v++) {
				for (int u = -radius; u <= radius; u++) {
					if (u * u + v * v <= radius * radius) {
						setTile(x + u, y + v, AIR);
						setMass(x + u, y + v, 0.0f);
					}
				}
			}
		} else if (mouse_m) {
			int x = mouse_x / 8;
			int y = mouse_y / 8;
			int radius = 2;
			for (int v = -radius; v <= radius; v++) {
				for (int u = -radius; u <= radius; u++) {
					if (u * u + v * v <= radius * radius) {
						setTile(x + u, y + v, SOLID);
						setMass(x + u, y + v, 0.0f);
					}
				}
			}
		}

		for (int q = 0; q < xr * yr; q++) {
			int x = rand() % xr;
			int y = rand() % yr;

			int tile = getTile(x, y);
			if (tile == LIQUID) {
				// Get mass.
				float mass = getMass(x, y);

				// Remove empty tiles.
				if (mass < 0.01f && false) {
					setTile(x, y, AIR);
					setMass(x, y, 0.0f);
					continue;
				}

				// Get below.
				int belowTile = getTile(x, y + 1);

				// If there is air below, move down one.
				if (belowTile == AIR) {
					setTile(x, y, AIR);
					setMass(x, y, 0.0f);
					setTile(x, y + 1, LIQUID);
					setMass(x, y + 1, mass);
				}

				// If there is water below that is not maxed out, transfer as much water as possible to the bottom.
				else {
					float belowMass = getMass(x, y + 1);
					if (belowTile == LIQUID && belowMass < 1.0f) {
						float belowNeeded = 1.0f - belowMass;
						if (mass < belowNeeded) {
							// Give all to the bottom.
							setTile(x, y, AIR);
							setMass(x, y, 0.0f);
							setMass(x, y + 1, belowMass + mass);
						} else {
							// Give some to the bottom.
							setMass(x, y, mass - belowMass);
							setMass(x, y + 1, 1.0f);
						}
					}

					// Evenly distribute between neighbours.
					else {
						bool canDistrLeft = getTile(x - 1, y) != SOLID;
						bool canDistrRight = getTile(x + 1, y) != SOLID;
						if (getTile(x - 1, y) == LIQUID && getMass(x - 1, y) > 0.95f) {
							canDistrLeft = false;
						}
						if (getTile(x + 1, y) == LIQUID && getMass(x + 1, y) > 0.95f) {
							canDistrRight = false;
						}
						if (canDistrLeft && canDistrRight) {
							// Set left, this and right to the average.
							float avg = (mass + getMass(x - 1, y) + getMass(x + 1, y)) / 3.0f;
							setTile(x - 1, y, LIQUID);
							setTile(x + 1, y, LIQUID);
							setMass(x, y, avg);
							setMass(x - 1, y, avg);
							setMass(x + 1, y, avg);
						} else if (canDistrLeft) {
							// Set left and this to the average.
							float avg = (mass + getMass(x - 1, y)) / 2.0f;
							setTile(x - 1, y, LIQUID);
							setMass(x, y, avg);
							setMass(x - 1, y, avg);
						} else if (canDistrRight) {
							// Set right and this to the average.
							float avg = (mass + getMass(x + 1, y)) / 2.0f;
							setTile(x + 1, y, LIQUID);
							setMass(x, y, avg);
							setMass(x + 1, y, avg);
						}
					}
				}
			}
		}

		for (int y = 0; y < yr; y++) {
			for (int x = 0; x < xr; x++) {
				int tile = getTile(x, y);
				if (tile == SOLID) {
					frectrgb(x * 8, y * 8, 8, 8, rgb(0, 0, 0));
				} else if (tile == LIQUID) {
					int mass = getMass(x, y) * 8.0f;
					if (mass > 8) {
						mass = 8;
					} else if (mass < 0) {
						mass = 0;
					}
					frectrgb(x * 8, y * 8 + 8 - mass, 8, mass, rgb(0, 0, 255));
				}
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