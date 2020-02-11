// Emptyness? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

const int xr = 50;
const int yr = 50;

const int AIR = 0;
const int SOLID = 1;

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

struct AABB {
	float x;
	float y;
	float w;
	float h;
};

bool intersect(AABB a, AABB b) {
	return (
		a.x < b.x + b.w &&
		a.x + a.w >= b.x &&
		a.y < b.y + b.h &&
		a.y + a.h >= b.y
	);
}

struct game: boiler
{
	float px = 48;
	float py = 48;
	float pw = 32;
	float ph = 32;
	float pvx = 0.0f;
	float pvy = 0.0f;
	float speed = 1.0f;

	void steam() override
	{
		width = 16 * xr;
		height = 16 * yr;

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

	bool acc[4];

	void keydown(SDL_Event e) override {
		SDL_Keycode key = e.key.keysym.sym;
		if (key == SDLK_SPACE) {
			px = 48;
			py = 48;
		} else if (key == SDLK_a) {
			acc[0] = true;
		} else if (key == SDLK_d) {
			acc[1] = true;
		} else if (key == SDLK_w) {
			acc[2] = true;
		} else if (key == SDLK_s) {
			acc[3] = true;
		}
	}

	void keyup(SDL_Event e) override {
		SDL_Keycode key = e.key.keysym.sym;
		if (key == SDLK_a) {
			acc[0] = false;
		} else if (key == SDLK_d) {
			acc[1] = false;
		} else if (key == SDLK_w) {
			acc[2] = false;
		} else if (key == SDLK_s) {
			acc[3] = false;
		}
	}

	void draw() override
	{
		black();

		if (acc[0]) pvx -= speed;
		if (acc[1]) pvx += speed;
		if (acc[2]) pvy -= speed;
		if (acc[3]) pvy += speed;

		
		pvx *= 0.95f;
		pvy *= 0.95f;

		if (mouse_l) {
			setTile(mouse_x / 16, mouse_y / 16, SOLID);
		} else if (mouse_r) {
			setTile(mouse_x / 16, mouse_y / 16, AIR);
		}

		// get all intersecting solid tiles
		std::vector<AABB> intersecting;
		for (int y = py / 16; y < (py + ph) / 16; y++) {
			for (int x = px / 16; x < (px + pw) / 16; x++) {
				if (getTile(x, y) == SOLID) {
					intersecting.push_back({x * 16.0f, y * 16.0f, 16.0f, 16.0f});
				}
			}
		}

		for (int y = 0; y < yr; y++) {
			for (int x = 0; x < xr; x++) {
				if (getTile(x, y) == SOLID) {
					frectrgb(x * 16, y * 16, 16, 16, rgb(127, 127, 127));
				}
			}
		}


		for (int i = 0; i < intersecting.size(); i++) {
			AABB a = intersecting[i];
			rectrgb(a.x, a.y, a.w, a.h, rgb(0, 255, 0));
		}

		
		int precision = 1;
		for (int qqq = 0; qqq < precision; qqq++)
		{
			
			py += pvy/float(precision);
			AABB p = {px, py, pw, ph};

			for (int i = 0; i < intersecting.size(); i++) {
				AABB a = p;
				AABB b = intersecting[i];
				// Make sure they intersect
				if (!intersect(p, a)) {
					continue;
				}

				// get x depth
				float xDepth;
				if (a.y + a.h > b.y + b.h) {
					xDepth = b.y + b.h - a.y;
				} else {
					xDepth = b.y - a.y - a.h;
				}
				p.y += xDepth;
				pvy = 0.0f;

				rectrgb(b.x, b.y, b.w, b.h, rgb(255, 0, 255));
			}
			px = p.x;
			py = p.y;

			px += pvx/float(precision);
			 p = {px, py, pw, ph};
			for (int i = 0; i < intersecting.size(); i++) {
				AABB a = p;
				AABB b = intersecting[i];
				// Make sure they intersect
				if (!intersect(p, a)) {
					continue;
				}

				// get x depth
				float xDepth;
				if (a.x + a.w > b.x + b.w) {
					xDepth = b.x + b.w - a.x;
				} else {
					xDepth = b.x - a.x - a.w;
				}
				p.x += xDepth;
				pvx = 0.0f;

				rectrgb(b.x, b.y, b.w, b.h, rgb(255, 0, 255));
			}




			px = p.x;
			py = p.y;
		}
		rectrgb(px, py, pw, ph, rgb(255, 0, 0));
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