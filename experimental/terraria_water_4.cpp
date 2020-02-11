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

int getTile(int x, int y) {
	if (x < 0 || y < 0 || x >= xr || y >= yr) {
		return 0;
	}
	return tiles[y * xr + x];
}

void setTile(int x, int y, int v) {
	if (x < 0 || y < 0 || x >= xr || y >= yr) {
		return;
	}
	tiles[y * xr + x] = v;
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
					}
				}
			}
		}

		for (int q = 0; q < xr * yr; q++) {
			int x = rand() % xr;
			int y = rand() % yr;

			int tile = getTile(x, y);
			if (tile == LIQUID) {
				bool tileBottom = getTile(x, y + 1) == AIR;
				if (tileBottom) {
					setTile(x, y, AIR);
					setTile(x, y + 1, LIQUID);
				} else {
					bool tileLeft = getTile(x - 1, y) == AIR;
					bool tileRight = getTile(x + 1, y) == AIR;
					if (tileLeft && tileRight) {
						if (rand() % 2) {
							setTile(x, y, AIR);
							setTile(x - 1, y, LIQUID);
						} else {
							setTile(x, y, AIR);
							setTile(x + 1, y, LIQUID);
						}
					} else if (tileLeft) {
						setTile(x, y, AIR);
						setTile(x - 1, y, LIQUID);
					} else if (tileRight) {
						setTile(x, y, AIR);
						setTile(x + 1, y, LIQUID);
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
					frectrgb(x * 8, y * 8, 8, 8, rgb(0, 0, 255));
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