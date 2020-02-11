// Emptyness? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

#define min fmin

float constrain(float x, float lower, float upper) {
	if (x < lower) {
		return lower;
	} else if (x > upper) {
		return upper;
	}
	return x;
}

const int xr = 100;
const int yr = 100;

const int AIR = 0;
const int SOLID = 1;
const int FLUID = 2;

int tiles[xr * yr];
float fluid1[xr * yr];
float fluid2[xr * yr];

int getTile(int x, int y) {
	if (x < 0 || y < 0 || x >= xr || y >= yr) {
		return SOLID;
	}
	return tiles[y * xr + x];
}

void setTile(int x, int y, int tile) {
	if (x < 0 || y < 0 || x >= xr || y >= yr) {
		return;
	}
	tiles[y * xr + x] = tile;
}

float getFluid1(int x, int y) {
	if (x < 0 || y < 0 || x >= xr || y >= yr) {
		return 0.0f;
	}
	return fluid1[y * xr + x];
}

void setFluid1(int x, int y, float fluid) {
	if (x < 0 || y < 0 || x >= xr || y >= yr) {
		return;
	}
	fluid1[y * xr + x] = fluid;
}

float getFluid2(int x, int y) {
	if (x < 0 || y < 0 || x >= xr || y >= yr) {
		return 0.0f;
	}
	return fluid2[y * xr + x];
}

void setFluid2(int x, int y, float fluid) {
	if (x < 0 || y < 0 || x >= xr || y >= yr) {
		return;
	}
	fluid2[y * xr + x] = fluid;
}

const float maxMass = 1.0f;
const float minMass = 0.0001f;
const float minDraw = 0.01f;
const float maxDraw = 1.1f;
const float maxCompress = 0.02f;
const float maxSpeed = 1.0f;
const float minFlow = 0.01f;

float getStableState(float totalMass) {
	if (totalMass <= 1.0f) {
		return 1.0f;
	} else if (totalMass < 2.0f * maxMass + maxCompress) {
		return (maxMass * maxMass + totalMass * maxCompress) / (maxMass + maxCompress);
	} else {
		return (totalMass + maxCompress) / 2.0f;
	}
}

void doFluid() {
	for (int x = 0; x < xr; x++) {
		for (int y = 0; y < yr; y++) {
			if (getTile(x, y) == SOLID) {
				continue;
			}

			float flow = 0.0f;
			float remainingMass = getFluid1(x, y);
			if (remainingMass <= 0.0f) {
				continue;
			}

			if (getTile(x, y + 1) != SOLID) {
				flow = getStableState(remainingMass + getFluid1(x, y + 1)) - getFluid1(x, y + 1);
				if (flow > minFlow) {
					flow *= 0.5f;
				}
				flow = constrain(flow, 0.0f, min(maxSpeed, remainingMass));

				setFluid2(x, y, getFluid2(x, y) - flow);
				setFluid2(x, y + 1, getFluid2(x, y + 1) + flow);
				remainingMass -= flow;
			}
			if (remainingMass <= 0.0f) {
				continue;
			}

			if (getTile(x - 1, y) != SOLID) {
				flow = (getFluid1(x, y) - getFluid1(x - 1, y)) / 4.0f;
				if (flow > minFlow) {
					flow *= 0.5f;
				}
				flow = constrain(flow, 0.0f, remainingMass);

				setFluid2(x, y, getFluid2(x, y) - flow);
				setFluid2(x - 1, y, getFluid2(x - 1, y) + flow);
				remainingMass -= flow;
			}
			if (remainingMass <= 0.0f) {
				continue;
			}

			if (getTile(x + 1, y) != SOLID) {
				flow = (getFluid1(x, y) - getFluid1(x + 1, y)) / 4.0f;
				if (flow > minFlow) {
					flow *= 0.5f;
				}
				flow = constrain(flow, 0.0f, remainingMass);

				setFluid2(x, y, getFluid2(x, y) - flow);
				setFluid2(x + 1, y, getFluid2(x + 1, y) + flow);
				remainingMass -= flow;
			}
			if (remainingMass <= 0.0f) {
				continue;
			}

			if (getTile(x, y - 1) != SOLID) {
				flow = remainingMass - getStableState(remainingMass + getFluid1(x, y - 1));
				if (flow > minFlow) {
					flow *= 0.5f;
				}
				flow = constrain(flow, 0.0f, min(maxSpeed, remainingMass));

				setFluid2(x, y, getFluid2(x, y) - flow);
				setFluid2(x, y - 1, getFluid2(x, y - 1) + flow);
				remainingMass -= flow;
			}
		}
	}

	// Copy new mass.
	memcpy(fluid1, fluid2, xr * yr * sizeof(float));

	// Update tile states.
	for (int x = 0; x < xr; x++) {
		for (int y = 0; y < yr; y++) {
			if (getTile(x, y) == SOLID) {
				continue;
			}

			if (getFluid1(x, y) > minMass) {
				setTile(x, y, FLUID);
			} else {
				setTile(x, y, AIR);
			}
		}
	}
}

struct game: boiler
{	
	void steam() override
	{
		width = 8 * xr;
		height = 8 * yr;

		title = "Emptyness (using Boiler)";
	}

	void draw() override
	{
		clear(rgb(255, 255, 255));

		doFluid();

		if (mouse_l) {
			int x = mouse_x / 8;
			int y = mouse_y / 8;
			int radius = 4;
			for (int v = -radius; v <= radius; v++) {
				for (int u = -radius; u <= radius; u++) {
					if (u * u + v * v <= radius * radius) {
						setTile(x + u, y + v, FLUID);
						setFluid1(x + u, y + v, maxMass);
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
						setFluid1(x + u, y + v, 0.0f);
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
						setFluid1(x + u, y + v, 0.0f);
					}
				}
			}
		}

		for (int y = 0; y < yr; y++) {
			for (int x = 0; x < xr; x++) {
				if (getTile(x, y) == SOLID) {
					frectrgb(x * 8, y * 8, 8, 8, rgb(0, 0, 0));
				} else {
					if (getFluid1(x, y) < maxMass) {
						if (getFluid1(x, y - 1) >= minDraw) {
							frectrgb(x * 8, y * 8, 8, 8, rgb(0, 0, 255));
						} else {
							int j = getFluid1(x, y) * 8;
							int i = constrain(j, 0, 8);
							frectrgb(x * 8, y * 8 - i + 8, 8, i, rgb(0, 0, 255));
						}
					} else {
						frectrgb(x * 8, y * 8, 8, 8, rgb(0, 0, 255));
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