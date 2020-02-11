// Emptyness? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

struct Hitbox {
	float x;
	float y;
	float w;
	float h;
};

std::vector<Hitbox> obstacles = {
	{
		800.0f / 2.0f - 100.0f / 2.0f,
		800.0f / 2.0f - 100.0f / 2.0f,
		100.0f,
		100.0f
	},
	{
		800.0f / 2.0f - 100.0f / 2.0f + 100.0f,
		800.0f / 2.0f - 100.0f / 2.0f,
		100.0f,
		100.0f
	},
	{
		800.0f / 2.0f - 100.0f / 2.0f - 100.0f,
		800.0f / 2.0f - 100.0f / 2.0f,
		100.0f,
		100.0f
	}
};

float px = 800.0f / 2.0f - 30.0f / 2.0f;
float py = 800.0f / 2.0f - 30.0f / 2.0f - 150.0f;
const float pw = 30.0f;
const float ph = 60.0f;
float pvx = 0.0f;
float pvy = 0.0f;
float acceleration = 5.0f;
float drag = 0.9f;

bool kw = false;
bool ka = false;
bool ks = false;
bool kd = false;

bool intersect(Hitbox a, Hitbox b) {
	return (
		(a.x < b.x + b.w && a.x + a.w > b.x) &&
		(a.y < b.y + b.h && a.y + a.h > b.y)
	);
}

struct game: boiler
{	
	void steam() override
	{
		width = 800;
		height = 800;

		title = "Emptyness (using Boiler)";
	}

	void keydown(SDL_Event e) override {
		SDL_Keycode key = e.key.keysym.sym;
		if (key == SDLK_w) kw = true;
		if (key == SDLK_a) ka = true;
		if (key == SDLK_s) ks = true;
		if (key == SDLK_d) kd = true;
	}

	void keyup(SDL_Event e) override {
		SDL_Keycode key = e.key.keysym.sym;
		if (key == SDLK_w) kw = false;
		if (key == SDLK_a) ka = false;
		if (key == SDLK_s) ks = false;
		if (key == SDLK_d) kd = false;
	}

	void draw() override
	{
		black();

		if (ka) pvx -= acceleration;
		if (kd) pvx += acceleration;
		if (kw) pvy -= acceleration;
		if (ks) pvy += acceleration;

		pvx *= drag;
		pvy *= drag;

		const int precision = 100;
		const float knockback = 8.0f;
		for (int i = 0; i < precision; i++) {
			{
				float opx = px;

				px += pvx / float(precision);

				Hitbox player = {px, py, pw, ph};

				for (int j = 0; j < obstacles.size(); j++) {
					Hitbox obstacle = obstacles[j];
					if (intersect(obstacle, player)) {
						px = opx;
					}
				}
			}
			{
				float opy = py;

				py += pvy / float(precision);

				Hitbox player = {px, py, pw, ph};

				for (int j = 0; j < obstacles.size(); j++) {
					Hitbox obstacle = obstacles[j];
					if (intersect(obstacle, player)) {
						py = opy;
					}
				}
			}
		}

		for (int j = 0; j < obstacles.size(); j++) {
			Hitbox obstacle = obstacles[j];
			rectrgb(obstacle.x, obstacle.y, obstacle.w, obstacle.h, rgb(255, 255, 255));
		}
		rectrgb(px, py, pw, ph, rgb(0, 255, 0));
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