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

int tiles[yr + 2][xr + 2];
int liquid[yr + 2][xr + 2];
bool falling[yr + 2][xr + 2];

struct game: boiler
{	
	void steam() override
	{
		width = 8 * xr;
		height = 8 * yr;

		title = "Emptyness (using Boiler)";

		for (int i = 1; i <= xr; i++) {
			tiles[1][i] = SOLID;
			tiles[yr][i] = SOLID;
		}
		for (int j = 1; j <= yr; j++) {
			tiles[j][1] = SOLID;
			tiles[j][xr] = SOLID;
		}

		memset(falling, 0, (xr + 2) * (yr + 2) * sizeof(bool));
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
						int i = x + u;
						int j = y + v;
						tiles[j + 1][i + 1] = LIQUID;
						liquid[j + 1][i + 1] = 8;
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
						int i = x + u;
						int j = y + v;
						tiles[j + 1][i + 1] = AIR;
						liquid[j + 1][i + 1] = 0;
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
						int i = x + u;
						int j = y + v;
						tiles[j + 1][i + 1] = SOLID;
						liquid[j + 1][i + 1] = 0;
					}
				}
			}
		}

		memset(falling, 0, (xr + 2) * (yr + 2) * sizeof(bool));

		// Left to right, transfer left.
		for (int z = 0; z < xr * yr * 10; z++) {
			int j = rand() % yr + 1;
			int i = rand() % xr + 1;
			int tile = tiles[j][i];

			if (tile == LIQUID) {
				if (liquid[j][i] == 0) {
					tiles[j][i] = AIR;
					continue;
				}

				int bottomTile = tiles[j + 1][i];
				int bottomLiquid = liquid[j + 1][i];
				if (bottomTile == SOLID || (bottomTile == LIQUID && bottomLiquid == 8)) {
					int thisLiquid = liquid[j][i];
					int leftTile = tiles[j][i - 1];
					int leftLiquid = liquid[j][i - 1];
					int rightTile = tiles[j][i + 1];
					int rightLiquid = liquid[j][i + 1];
					bool left = leftTile == SOLID || (leftTile == LIQUID && leftLiquid >= thisLiquid);
					bool right = rightTile == SOLID || (rightTile == LIQUID && rightLiquid >= thisLiquid);
					if (left && right) {
						if (liquid[j][i] <= 2) {
							tiles[j][i] = AIR;
							liquid[j][i] = 0;
							continue;
						}
					} else if (left) {
						// Give one to right.
						liquid[j][i]--;
						tiles[j][i + 1] = LIQUID;
						liquid[j][i + 1]++;
					} else if (right) {
						// Give one to left.
						liquid[j][i]--;
						tiles[j][i - 1] = LIQUID;
						liquid[j][i - 1]++;
					} else {
						if (thisLiquid > 1) {
							// Give one to right.
							liquid[j][i]--;
							tiles[j][i + 1] = LIQUID;
							liquid[j][i + 1]++;
							// Give one to left.
							liquid[j][i]--;
							tiles[j][i - 1] = LIQUID;
							liquid[j][i - 1]++;
						} else {
							// Pick a random side to distribute to.
							if (rand() % 2) {
								// Give one to right.
								liquid[j][i]--;
								tiles[j][i + 1] = LIQUID;
								liquid[j][i + 1]++;
							} else {
								// Give one to left.
								liquid[j][i]--;
								tiles[j][i - 1] = LIQUID;
								liquid[j][i - 1]++;
							}
						}
					}
				} else {
					// Give one to bottom.
					liquid[j][i]--;
					tiles[j + 1][i] = LIQUID;
					liquid[j + 1][i]++;
				}
			}
		}

		for (int j = 1; j <= yr; j++) {
			for (int i = 1; i <= xr; i++) {
				int j_ = j - 1;
				int i_ = i - 1;

				int tile = tiles[j][i];
				if (tile == SOLID) {
					frectrgb(i_ * 8, j_ * 8, 8, 8, rgb(0, 0, 0));
				} else if (tile == LIQUID) {
					int mass = liquid[j][i];
					if (mass <= 0) {
						frectrgb(i_ * 8, j_ * 8, 8, 8, rgb(255, 255, 0));
					} else if (mass > 8) {
						frectrgb(i_ * 8, j_ * 8, 8, 8, rgb(255, 0, 0));
					} else if (mass == 8) {
						frectrgb(i_ * 8, j_ * 8, 8, 8, rgb(0, 0, 255));
					} else {
						frectrgb(i_ * 8, j_ * 8 + 8 - mass, 8, mass, rgb(0, 0, 255));
					}
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