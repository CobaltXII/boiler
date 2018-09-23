/*

balls.cpp

This is a simple, inaccurate and fun toy for simulating the physics of rigid circular bodies. This
simulation supports circle vs. circle collisions, along with circle vs. capsule collisions. It 
also supports momentum conservation, displacement and gravity. This simulation was written 
entirely by me (@CobaltXII#4538), although I would like to thank a few people for helping me out 
quite a lot while writing this code:

	@javidx9#3371, for creating three invaluable tutorials

		https://www.youtube.com/watch?v=LPzyNOHY3A4
		https://www.youtube.com/watch?v=ebq7L2Wtbl4
		https://www.youtube.com/watch?v=ti1DBybw1o4 

	and @huhlig#8383, for helping me fix a really annoying bug.

This code was made for fun, you can do whatever you want with it as long as you credit me and
include my Discord tag (@CobaltXII#4538). This code is licensed under the GNU GPLv3 license, a
copy of which is provided with this source file.

All rights reserved.

===================================================================================================

Left mouse button						Shoot around balls using a cue
Right mouse button						Modify line end caps

Q 										Attract balls to the mouse using gravity
W 										Repel balls from the mouse using gravity

SPACE 									Cause balls to rain from the sky
RETURN 									Toggle debug mode

===================================================================================================

Build with

	g++ $1 -std=c++11 -lSDL2 -Ofast

*/

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

bool debug = true;

struct ball
{
	// Variables that start with 'p' are for the position, variables that start with 'v' are for
	// the velocity, and variables that start with 'a' are for the acceleration.

	double px;
	double py;

	double vx;
	double vy;

	double rad;
	double mass;

	int id;
};

struct line
{
	// The variables that start with 'a' represent the first point in the line, and the variables
	// that start with 'b' represent the second point in the line. 

	double ax;
	double ay;

	double bx;
	double by;

	double rad;
};

struct game: boiler
{	
	int unique_id = 0;

	// The list of balls that will be tested throughout the program. All of these balls can
	// collide with any other ball in this vector.

	std::vector<ball> balls;
	std::vector<line> lines;

	// The pointer to the currently selected ball. This is used so that we can shoot balls around
	// using mouse-based cues.

	ball* s_ball = nullptr;

	// The pointer to the coordinates of the currently selected line cap. This is used so that we
	// can modify line caps in realtime.

	double* s_line_x = nullptr;
	double* s_line_y = nullptr;

	// Add a ball with no velocity or acceleration to the vector of balls.

	void add_ball(double x, double y, double vx, double vy, double rad)
	{
		ball n_ball;

		n_ball.px = x;
		n_ball.py = y;

		n_ball.vx = vx;
		n_ball.vy = vy;

		n_ball.rad = rad;

		n_ball.mass = rad * 1.0;

		// Get an identifier for the ball. This identifier is a copy of the current value of
		// unique_id, which is incremented.

		n_ball.id = unique_id++;

		// Push the ball to the back of the ball vector.

		balls.push_back(n_ball);
	}

	// Add a line to the vector of lines.

	void add_line(double ax, double ay, double bx, double by, double rad)
	{
		line n_line;

		n_line.ax = ax;
		n_line.ay = ay;

		n_line.bx = bx;
		n_line.by = by;

		n_line.rad = rad;

		// Push the line to the back of the line vector.

		lines.push_back(n_line);
	}

	// Initialize Boiler.

	void steam() override
	{
		width = 800;
		height = 600;

		title = "Programming balls (using Boiler)";

		// Seed the random number generator using the current timestamp.

		srand((unsigned int)time(NULL));

		// Place some random balls around the scene.

		for (int i = 0; i < 600; i++)
		{
			add_ball
			(
				rand() % width,
				rand() % height,

				((rand() % 10000) - 5000) / 500.0,
				((rand() % 10000) - 5000) / 500.0,

				(((double)(rand() % RAND_MAX)) / RAND_MAX) * 5.0 + 2.0
			);
		}

		// Add some lines.

		for (int i = 0; i < 10; i++)
		{
			add_line
			(
				rand() % width,
				rand() % height,

				rand() % width,
				rand() % height,

				5.0
			);
		}
	}

	// Handle a keypress using Boiler.

	void keydown(SDL_Event e) override
	{
		if (e.key.keysym.sym == SDLK_SPACE)
		{
			// Remember the amount of balls currently inside the system, and then clear the 
			// system. Then add the same amount of balls back into the system.

			int c_balls = balls.size();

			balls.clear();

			for (int i = 0; i < c_balls; i++)
			{
				add_ball
				(
					width / 2,
					
					10.0 * -i,

					0.0,
					0.0,

					(((double)(rand() % RAND_MAX)) / RAND_MAX) * 5.0 + 2.0
				);
			}
		}
		else if (e.key.keysym.sym == SDLK_RETURN)
		{
			debug = !debug;
		}
	}

	// Render a frame using Boiler.

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		// Get the keyboard state.

		const Uint8* state = SDL_GetKeyboardState(NULL);

		// Blender.

		if (false)
		{
			line& l_blender = lines[0];

			l_blender.ax = width / 2;
			l_blender.ay = height / 2;

			l_blender.bx = sin(iteration / 50.0) * 70.0 + l_blender.ax;
			l_blender.by = cos(iteration / 50.0) * 70.0 + l_blender.ay;
		}

		// Allow balls to be shot around by the mouse, and line caps to be moved around by the
		// mouse.

		if (mouse_r == true && s_line_x == nullptr && s_line_y == nullptr)
		{
			for (int i = 0; i < lines.size(); i++)
			{
				line& i_line = lines[i];

				if
				(
					fabs
					(
						(i_line.ax - mouse_x) * (i_line.ax - mouse_x) +
						(i_line.ay - mouse_y) * (i_line.ay - mouse_y)
					)

					<

					(i_line.rad * i_line.rad)
				)
				{
					s_line_x = &i_line.ax;
					s_line_y = &i_line.ay;

					break;
				}

				if
				(
					fabs
					(
						(i_line.bx - mouse_x) * (i_line.bx - mouse_x) +
						(i_line.by - mouse_y) * (i_line.by - mouse_y)
					)

					<

					(i_line.rad * i_line.rad)
				)
				{
					s_line_x = &i_line.bx;
					s_line_y = &i_line.by;

					break;
				}
			}
		}
		else if (mouse_r == false)
		{
			// Nullify the pointers.

			s_line_x = nullptr;
			s_line_y = nullptr;
		}
		else
		{
			if (s_line_x != nullptr && s_line_y != nullptr)
			{
				*s_line_x = mouse_x;
				*s_line_y = mouse_y;
			}
		}

		if (mouse_l == true && s_ball == nullptr)
		{
			for (int i = 0; i < balls.size(); i++)
			{
				ball& i_ball = balls[i];

				if
				(
					fabs
					(
						(i_ball.px - mouse_x) * (i_ball.px - mouse_x) +
						(i_ball.py - mouse_y) * (i_ball.py - mouse_y)
					)

					<

					(i_ball.rad * i_ball.rad)
				)
				{
					s_ball = &i_ball;

					break;
				}
			}
		}
		else if (mouse_l == false)
		{
			if (s_ball != nullptr)
			{
				ball& c_ball = *s_ball;

				// Apply acceleration to the ball based on the mouse pointer's position relative
				// to the ball's positon.

				double dx = mouse_x - c_ball.px;
				double dy = mouse_y - c_ball.py;

				c_ball.vx = dx / 7;
				c_ball.vy = dy / 7;

				// Nullify the pointer, because this ball has been shot away somewhere and is not
				// selected anymore.

				s_ball = nullptr;
			}
		}

		std::vector<std::pair<ball&, ball&>> v_colliding;

		// Iterate many times per frame so that the precision of operations is greater.

		double d_factor = 20.0;

		for (int w = 0; w < d_factor; w++)
		{
			// Update ball positions.

			v_colliding.clear();

			for (int i = 0; i < balls.size(); i++)
			{
				ball& i_ball = balls[i];

				// Loop balls around if they go off-screen.

				/*

				if (i_ball.px >= width)
				{
					i_ball.px = 0;
				}

				if (i_ball.px < 0)
				{
					i_ball.px = width;
				}

				if (i_ball.py >= height)
				{
					i_ball.py = 0;
				}

				if (i_ball.py < 0)
				{
					i_ball.py = height;
				}

				*/

				// Apply drag (very fake) and modify position by velocity.

				i_ball.vx = i_ball.vx / (pow(1.05, (1.0 / d_factor)));
				i_ball.vy = i_ball.vy / (pow(1.05, (1.0 / d_factor)));

				i_ball.px += i_ball.vx / d_factor;
				i_ball.py += i_ball.vy / d_factor;
			
				if (state[SDL_SCANCODE_Q] == true)
				{
					double dx = mouse_x - i_ball.px;
					double dy = mouse_y - i_ball.py;

					double dz = dx * dx + dy * dy;

					if (dz <= 10000.0)
					{
						dz = 10000.0;
					}

					i_ball.vx += (dx * (90 / dz)) / d_factor;
					i_ball.vy += (dy * (90 / dz)) / d_factor;
				}
				else if (state[SDL_SCANCODE_W] == true)
				{
					double dx = mouse_x - i_ball.px;
					double dy = mouse_y - i_ball.py;

					double dz = dx * dx + dy * dy;

					if (dz <= 10000.0)
					{
						dz = 10000.0;
					}

					i_ball.vx += (dx * (-90 / dz)) / d_factor;
					i_ball.vy += (dy * (-90 / dz)) / d_factor;
				}

				// Apply downwards gravity.

				i_ball.vy += (2.5 / i_ball.rad) / d_factor;

				// Treat borders of screen as solids.	

				if (i_ball.py + i_ball.rad > height)
				{
					i_ball.py -= (i_ball.py + i_ball.rad) - height;

					i_ball.vy = -i_ball.vy;
				}

				if (i_ball.px + i_ball.rad > width)
				{
					i_ball.px -= (i_ball.px + i_ball.rad) - width;

					i_ball.vx = -i_ball.vx;
				}

				if (i_ball.px - i_ball.rad < 0)
				{
					i_ball.px -= (i_ball.px - i_ball.rad);

					i_ball.vx = -i_ball.vx;
				}

				// Make sure balls can't go too fast.

				double m_speed = i_ball.rad * 2.0;

				double v_len =
				(
					i_ball.vx * i_ball.vx +
					i_ball.vy * i_ball.vy
				);

				if (v_len > m_speed * m_speed)
				{
					i_ball.vx = (i_ball.vx / v_len) * (m_speed * m_speed);
					i_ball.vy = (i_ball.vy / v_len) * (m_speed * m_speed);
				}
			}

			// Do collisions.

			for (int u = 0; u < balls.size(); u++)
			{
				ball& u_ball = balls[u];

				// Against lines.

				for (int v = 0; v < lines.size(); v++)
				{
					line& v_line = lines[v];

					double lx1 = v_line.bx - v_line.ax;
					double ly1 = v_line.by - v_line.ay;

					double lx2 = u_ball.px - v_line.ax;
					double ly2 = u_ball.py - v_line.ay;

					double l_len = 
					(
						lx1 * lx1 +
						ly1 * ly1
					);

					double d_t = 
					(
						std::max
						(
							0.0,

							std::min
							(
								l_len,

								(
									lx1 * lx2 +
									ly1 * ly2
								)
							)
						)

						/ l_len
					);

					double cpx = v_line.ax + d_t * lx1;
					double cpy = v_line.ay + d_t * ly1;

					double d_dist = sqrt
					(
						(u_ball.px - cpx) * (u_ball.px - cpx) +
						(u_ball.py - cpy) * (u_ball.py - cpy)
					);

					if (d_dist <= (u_ball.rad + v_line.rad))
					{
						// Collision between u_ball and v_line. Treat the line as a rigid ball
						// cannot move.

						if (w == d_factor - 1 && debug)
						{
							linergb
							(
								u_ball.px,
								u_ball.py,

								cpx,
								cpy,

								rgb(0, 255, 0)
							);
						}

						double d_overlap =
						(
							d_dist -

							u_ball.rad -
							v_line.rad
						);

						u_ball.px -= d_overlap * (u_ball.px - cpx) / d_dist;
						u_ball.py -= d_overlap * (u_ball.py - cpy) / d_dist;
					}
				}

				// Against other balls.

				for (int v = 0; v < balls.size(); v++)
				{
					ball& v_ball = balls[v];

					if (u_ball.id != v_ball.id)
					{
						// Check if u_ball and v_ball are intersecting. They are intersecting if the
						// square of their distance to each other is less than or equal to the sum of
						// their radii squared.

						double dx = (u_ball.px - v_ball.px) * (u_ball.px - v_ball.px);
						double dy = (u_ball.py - v_ball.py) * (u_ball.py - v_ball.py);

						bool b_intersect = 
						(
							fabs
							(
								dx + dy
							)

							<=

							(u_ball.rad + v_ball.rad) *
							(u_ball.rad + v_ball.rad)
						);

						if (b_intersect)
						{
							// Find the distance between u_ball and v_ball. This can be optimized by
							// reusing values, but we won't do that right now.

							double d_dist = sqrt
							(
								dx + dy
							);

							// Find out how much the balls are overlapping. We only want half of this
							// value, so the overlap is halved.

							double d_overlap = 
							(
								(
									d_dist - 

									u_ball.rad - 
									v_ball.rad
								)

								* 0.5
							);

							// Displace u_ball.

							u_ball.px -= d_overlap * (u_ball.px - v_ball.px) / d_dist;
							u_ball.py -= d_overlap * (u_ball.py - v_ball.py) / d_dist;

							// Displace v_ball.

							v_ball.px += d_overlap * (u_ball.px - v_ball.px) / d_dist;
							v_ball.py += d_overlap * (u_ball.py - v_ball.py) / d_dist;

							// Add u_ball and v_ball to the colliding pairs vector as a new pair.

							v_colliding.push_back
							(
								std::pair<ball&, ball&>
								(
									u_ball, 
									v_ball
								)
							);
						}
					}
				}
			}

			// Do dynamic collisions.

			for (int i = 0; i < v_colliding.size(); i++)
			{
				ball& a_ball = v_colliding[i].first;
				ball& b_ball = v_colliding[i].second;

				// Distance between a_ball and b_ball.

				double d_dist = sqrt
				(
					(a_ball.px - b_ball.px) * (a_ball.px - b_ball.px) +
					(a_ball.py - b_ball.py) * (a_ball.py - b_ball.py)
				);

				// Normal from a_ball to b_ball.

				double nx = (b_ball.px - a_ball.px) / d_dist;
				double ny = (b_ball.py - a_ball.py) / d_dist;

				double kx = (a_ball.vx - b_ball.vx);
				double ky = (a_ball.vy - b_ball.vy);

				double p = 2.0 * (nx * kx + ny * ky) / (a_ball.mass + b_ball.mass);

				a_ball.vx = (a_ball.vx - p * b_ball.mass * nx);
				a_ball.vy = (a_ball.vy - p * b_ball.mass * ny);
				b_ball.vx = (b_ball.vx + p * a_ball.mass * nx);
				b_ball.vy = (b_ball.vy + p * a_ball.mass * ny);
			}
		}

		// Draw all the visible balls in the scene. Draw a velocity vector in red.  The ball 
		// should be white.

		for (int i = 0; i < balls.size(); i++)
		{
			ball d_ball = balls[i];

			circlergb
			(
				d_ball.px,
				d_ball.py,

				d_ball.rad,

				rgb(255, 255, 255)
			);

			if (debug)
			{
				// Calculate the end position of the velocity vector line.

				int vex = d_ball.px + d_ball.vx;
				int vey = d_ball.py + d_ball.vy;

				linergb
				(
					d_ball.px,
					d_ball.py,

					vex,
					vey,

					rgb(255, 0, 0)
				);
			}
		}

		// Draw all the visible lines in the scene. The line should be white.

		for (int i = 0; i < lines.size(); i++)
		{
			line d_line = lines[i];

			circlergb
			(
				d_line.ax,
				d_line.ay,

				d_line.rad,

				rgb(255, 255, 255)
			);

			circlergb
			(
				d_line.bx,
				d_line.by,

				d_line.rad,

				rgb(255, 255, 255)
			);

			double nx = 0 - (d_line.by - d_line.ay);
			double ny = 0 + (d_line.bx - d_line.ax);

			double d = sqrt
			(
				nx * nx +
				ny * ny
			);

			nx /= d;
			ny /= d;

			linergb
			(
				d_line.ax + nx * d_line.rad,
				d_line.ay + ny * d_line.rad,

				d_line.bx + nx * d_line.rad,
				d_line.by + ny * d_line.rad,

				rgb(255, 255, 255)
			);

			linergb
			(
				d_line.ax - nx * d_line.rad,
				d_line.ay - ny * d_line.rad,

				d_line.bx - nx * d_line.rad,
				d_line.by - ny * d_line.rad,

				rgb(255, 255, 255)
			);
		}

		// Connect all colliding balls with a white line.

		if (debug)
		{
			for (int i = 0; i < v_colliding.size(); i++)
			{
				ball& a_ball = v_colliding[i].first;
				ball& b_ball = v_colliding[i].second;

				// Ignore line vs. ball collisions.

				if (b_ball.id != -1)
				{
					linergb
					(
						a_ball.px,
						a_ball.py,

						b_ball.px,
						b_ball.py,

						rgb(255, 255, 255)
					);
				}
			}
		}

		// Draw the cue from the mouse to the selected ball, if any.

		if (s_ball != nullptr)
		{
			ball c_ball = *s_ball;

			linergb
			(
				c_ball.px,
				c_ball.py,

				mouse_x,
				mouse_y,

				rgb(0, 0, 255)
			);
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