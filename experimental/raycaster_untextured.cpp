// Raycaster? Not awesome. This is garbage.
//
// https://lodev.org/cgtutor/raycasting.html

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

float pos_x = 22;
float pos_y = 12;

float dir_x = 0.0f - 1.0f;
float dir_y = 0.0f + 0.0f;

float plane_x = 0.0f;

float plane_y = 0.66f;

const int map_x_res = 24;
const int map_y_res = 24;

int map[map_x_res][map_y_res] =
{
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 4, 0, 0, 0, 0, 5, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 4, 0, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

struct game: boiler
{	
	void steam() override
	{
		width = 800;
		
		height = 600;

		title = "Raycaster (using Boiler)";
	}

	void draw() override
	{
		black();

		for (int x = 0; x < width; x++)
		{
			float camera_x = 2.0f * x / float(width) - 1.0f;

			float ray_dir_x = dir_x + plane_x * camera_x;
			float ray_dir_y = dir_y + plane_y * camera_x;

			int map_x = int(pos_x);
      		int map_y = int(pos_y);

			float side_dist_x;
			float side_dist_y;

			float delta_dist_x = std::abs(1.0f / ray_dir_x);
      		float delta_dist_y = std::abs(1.0f / ray_dir_y);

      		float perp_wall_dist;

			int step_x;
			int step_y;

			int hit = 0;

			int side = 0;

			if (ray_dir_x < 0.0f)
			{
				step_x = -1;

				side_dist_x = (pos_x - map_x) * delta_dist_x;
			}
			else
			{
				step_x = 1;

				side_dist_x = (map_x + 1.0f - pos_x) * delta_dist_x;
			}

			if (ray_dir_y < 0.0f)
			{
				step_y = -1;

				side_dist_y = (pos_y - map_y) * delta_dist_y;
			}
			else
			{
				step_y = 1;

				side_dist_y = (map_y + 1.0f - pos_y) * delta_dist_y;
			}

			while (hit == 0)
			{
				if (side_dist_x < side_dist_y)
				{
					side_dist_x += delta_dist_x;

					map_x += step_x;

					side = 0;
				}
				else
				{
					side_dist_y += delta_dist_y;

					map_y += step_y;

					side = 1;
				}

				if (map[map_x][map_y] > 0)
				{
					hit = 1;
				}
			}

			if (side == 0)
			{
				perp_wall_dist = (map_x - pos_x + (1 - step_x) / 2) / ray_dir_x;
			}
			else
			{
				perp_wall_dist = (map_y - pos_y + (1 - step_y) / 2) / ray_dir_y;
			}

			int line_height = (int)(height / perp_wall_dist * 4);

			int y1 = 0 - line_height / 2 + height / 2;
			int y2 = 0 + line_height / 2 + height / 2;

			if (y1 < 0)
			{
				y1 = 0;
			}

			if (y2 >= height)
			{
				y2 = height - 1;
			}

			unsigned int color;

			int tile = map[map_x][map_y];

			if (tile == 1)
			{
				color = rgb(255, 0, 0);
			}
			else if (tile == 1)
			{
				color = rgb(0, 255, 0);
			}
			else if (tile == 1)
			{
				color = rgb(0, 0, 255);
			}
			else if (tile == 1)
			{
				color = rgb(255, 0, 255);
			}
			else
			{
				color = rgb(0, 255, 255);
			}

			if (side == 1)
			{
				unsigned char nr = mgetr(color) / 2;
				unsigned char ng = mgetg(color) / 2;
				unsigned char nb = mgetb(color) / 2;

				color = rgb(nr, ng, nb);
			}

			unsigned int nr = mgetr(color) / perp_wall_dist * 8;
			unsigned int ng = mgetg(color) / perp_wall_dist * 8;
			unsigned int nb = mgetb(color) / perp_wall_dist * 8;

			color = rgb
			(
				std::min(nr, 255u), 
				std::min(ng, 255u), 
				std::min(nb, 255u)
			);

			for (int y = y1; y <= y2; y++)
			{
				plotp(x, y, color);
			}
		}

		const Uint8* keys = SDL_GetKeyboardState(NULL);

		float movement_speed = 5.0f / 60.0f;

		float rotation_speed = 3.0f / 60.0f;

		if (keys[SDL_SCANCODE_W])
		{
			if(map[int(pos_x + dir_x * movement_speed)][int(pos_y)] == 0)
			{
				pos_x += dir_x * movement_speed;
			}

			if(map[int(pos_x)][int(pos_y + dir_y * movement_speed)] == 0)
			{
				pos_y += dir_y * movement_speed;
			}
		}

		if (keys[SDL_SCANCODE_S])
		{
			if(map[int(pos_x - dir_x * movement_speed)][int(pos_y)] == 0)
			{
				pos_x -= dir_x * movement_speed;
			}

			if(map[int(pos_x)][int(pos_y - dir_y * movement_speed)] == 0)
			{
				pos_y -= dir_y * movement_speed;
			}
		}

		if (keys[SDL_SCANCODE_A])
		{
			float old_dir_x = dir_x;

			dir_x = dir_x * cos(rotation_speed) - dir_y * sin(rotation_speed);

			dir_y = old_dir_x * sin(rotation_speed) + dir_y * cos(rotation_speed);

			float old_plane_x = plane_x;

			plane_x = plane_x * cos(rotation_speed) - plane_y * sin(rotation_speed);

			plane_y = old_plane_x * sin(rotation_speed) + plane_y * cos(rotation_speed);
		}

		if (keys[SDL_SCANCODE_D])
		{
			float old_dir_x = dir_x;

			dir_x = dir_x * cos(-rotation_speed) - dir_y * sin(-rotation_speed);

			dir_y = old_dir_x * sin(-rotation_speed) + dir_y * cos(-rotation_speed);

			float old_plane_x = plane_x;

			plane_x = plane_x * cos(-rotation_speed) - plane_y * sin(-rotation_speed);

			plane_y = old_plane_x * sin(-rotation_speed) + plane_y * cos(-rotation_speed);
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