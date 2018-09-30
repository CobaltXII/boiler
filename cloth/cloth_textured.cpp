/*

Verlet integration? Using code from my barycentric triangle renderer.

https://www.youtube.com/watch?v=3HjO_RGIjCU
https://www.youtube.com/watch?v=pBMivz4rIJY&t=508s
https://www.youtube.com/watch?v=tAd7ttKbugA
https://www.youtube.com/watch?v=YgRZDCBLDfs

*/

#include "../boiler/boiler.h"

#include <vector>
#include <iostream>

// Clamp a value to the range 0 to 1.

inline double clampuv(double x)
{
	return std::max(0.0, std::min(1.0, x));
}

struct color
{
	Uint8 r;
	Uint8 g;
	Uint8 b;

	color(double _r, double _g, double _b)
	{
		r = clamprgb(_r * 255.0);
		g = clamprgb(_g * 255.0);
		b = clamprgb(_b * 255.0);
	}
};

struct uv
{
	double u;
	double v;

	uv(double _u, double _v)
	{
		u = _u;
		v = _v;
	}
};

// Naive velocity clamp.

double clampm(double val, double max)
{
	if (val > 0.0)
	{
		return std::min(val, max);
	}
	else
	{
		return std::max(val, -max);
	}
}

// A point.

struct point
{
	double px;
	double py;

	double opx;
	double opy;

	// If the locked flag is set to true, the point will not be able to move. This should also
	// change the effects of constraints and joints.

	bool locked = false;

	// Constructor, set the old position to the current position.

	point
	(
		double _px,
		double _py
	)
	{
		px = _px;
		py = _py;

		opx = _px;
		opy = _py;
	}

	// Constructor, set the old position to the current position. Optionally set the locked flag 
	// to true. There is no old and current position constructor with an optional lock flag, 
	// because that is redundant.

	point
	(
		double _px,
		double _py,

		bool _locked
	)
	{
		px = _px;
		py = _py;

		opx = _px;
		opy = _py;

		locked = _locked;
	}

	// Constructor, set the old position and the current position.

	point
	(
		double _px,
		double _py,

		double _opx,
		double _opy
	)
	{
		px = _px;
		py = _py;

		opx = _opx;
		opy = _opy;
	}
};

// A rigid constraint.

struct constraint
{
	point* p1;
	point* p2;

	double d;

	// This flag is set to false when a constraint is stretched too long. This flag is used 
	// instead of slowly removing a constraint from a vector.

	bool active = true;

	// Constructor, define the distance manually.

	constraint
	(
		point* _p1,
		point* _p2,

		double _d
	)
	{
		p1 = _p1;
		p2 = _p2;

		d = _d;
	}

	// Constructor, calculate the distance automatically.

	constraint
	(
		point* _p1,
		point* _p2
	)
	{
		p1 = _p1;
		p2 = _p2;

		double dx = p2->px - p1->px;
		double dy = p2->py - p1->py;

		d = sqrt
		(
			dx * dx +
			dy * dy
		);
	}

	// Affector.

	void do_constraint()
	{
		double dx = p2->px - p1->px;
		double dy = p2->py - p1->py;

		double cd = sqrt
		(
			dx * dx +
			dy * dy
		);

		// Calculate half of the difference between the constraint length and the current 
		// distance. Each point will be pushed away from each other by this factor.

		double nd = (d - cd) / cd / 2.0;

		double ox = dx * nd;
		double oy = dy * nd;

		if (p1->locked == false)
		{
			if (p2->locked == false)
			{		
				p1->px -= ox;
				p1->py -= oy;	

				p2->px += ox;
				p2->py += oy;
			}
			else
			{
				p1->px -= ox * 2.0;
				p1->py -= oy * 2.0;	
			}
		}
		else
		{
			if (p2->locked == false)
			{		
				p2->px += ox * 2.0;
				p2->py += oy * 2.0;
			}
		}

		// Break when stretched too long.

		if (nd * 2.0 < -1.0)
		{
			active = false;
		}
	}
};

struct game: boiler
{	
	// The image to texture the cloth with.

	int iw;
	int ih;

	Uint32* image = loadbmp("img/canada_large.bmp", iw, ih);

	// Simple shader. Treats vertex data as UV coordinates.

	static Uint32 my_shader
	(
		double px, double py,

		double tx1, double ty1,
		double tx2, double ty2,
		double tx3, double ty3,

		void* d1,
		void* d2,
		void* d3,

		game* g
	)
	{
		// The position of the light.

		double lx = 800.0 / 2.0;
		double ly = 600.0 / 2.0;

		double lz = 100.0;

		// Find the weights for each vertex using barycentric coordinates.

		double w1 = 
		(
			(
				(ty2 - ty3) * (px - tx3) +
				(tx3 - tx2) * (py - ty3)
			)

			/

			(
				(ty2 - ty3) * (tx1 - tx3) +
				(tx3 - tx2) * (ty1 - ty3) 
			)
		);

		double w2 = 
		(
			(
				(ty3 - ty1) * (px - tx3) +
				(tx1 - tx3) * (py - ty3)
			)

			/

			(
				(ty2 - ty3) * (tx1 - tx3) +
				(tx3 - tx2) * (ty1 - ty3)
			)
		);

		double w3 = 1.0 - w1 - w2;

		// Use the weights to calculate the UV coordinates of the current pixel.

		uv* uv1 = (uv*)d1;
		uv* uv2 = (uv*)d2;
		uv* uv3 = (uv*)d3;

		double cu = w1 * (double)(uv1->u) + w2 * (double)(uv2->u) + w3 * (double)(uv3->u); 
		double cv = w1 * (double)(uv1->v) + w2 * (double)(uv2->v) + w3 * (double)(uv3->v); 

		cu = clampuv(cu);
		cv = clampuv(cv);

		// Fetch the corresponding color from the texture.

		Uint32 c = g->image[(int)(cv * g->ih) * g->iw + (int)(cu * g->iw)];

		Uint8 rc = c >> 16;
		Uint8 gc = c >> 8;
		Uint8 bc = c >> 0;

		// Calculate lighting.

		double dx = lx - px;
		double dy = ly - py;

		double dz = lz;

		double ds = 
		(
			(dx * dx) +
			(dy * dy) +
			(dz * dz)
		);

		ds = std::min(1.0, 1.0 / ds * 25000.0);

		return rgb((double)rc * ds, (double)gc * ds, (double)bc * ds);
	}

	// Shade a triangle using a shader.

	void shadetriangle
	(
		int tx1, int ty1, 
		int tx2, int ty2, 
		int tx3, int ty3, 

		void* d1,
		void* d2,
		void* d3,

		Uint32 (*shader)
		(
			// The X and Y coordinates of the pixel to shade.

			double, double,

			// The coordinates of the three vertices of the triangle.

			double, double,
			double, double,
			double, double,

			// The vertex data associated with each of the three vertices of the triangle.

			void*, 
			void*, 
			void*,

			// The caller.

			game*
		)
	)
	{	
		int t1x; 
		int t2x;

		int y;

		int minx;
		int maxx;

		int t1xp; 
		int t2xp;

		bool changed1 = false;
		bool changed2 = false;

		int signx1;
		int signx2;

		int dx1;
		int dy1;

		int dx2;
		int dy2;

		int e1;
		int e2;

		int x1 = tx1;
		int y1 = ty1;

		int x2 = tx2;
		int y2 = ty2;

		int x3 = tx3;
		int y3 = ty3;
		
		if (y1 > y2) 
		{ 
			std::swap(y1, y2); 
			std::swap(x1, x2); 
		}
		
		if (y1 > y3) 
		{ 
			std::swap(y1, y3); 
			std::swap(x1, x3); 
		}
		
		if (y2 > y3)
		{ 
			std::swap(y2, y3); 
			std::swap(x2, x3); 
		}

		t1x = x1; 
		t2x = x1;

		y = y1;

		dx1 = (int)(x2 - x1);   
		dy1 = (int)(y2 - y1);

		if (dx1 < 0)
		{ 
			dx1 = -dx1; 

			signx1 = -1; 
		}
		else 
		{
			signx1 = 1;
		}		

		dx2 = (int)(x3 - x1);
		dy2 = (int)(y3 - y1);

		if (dx2 < 0) 
		{ 
			dx2 = -dx2; 

			signx2 = -1; 
		}
		else
		{
			signx2 = 1;
		}		

		if (dy1 > dx1) 
		{
			std::swap(dx1, dy1);

			changed1 = true;
		}
		if (dy2 > dx2) 
		{
			std::swap(dy2, dx2);

			changed2 = true;
		}

		e1 = (int)(dx1 >> 1);
		e2 = (int)(dx2 >> 1);
		
		if (y1 == y2)
		{
			goto next;
		}	

		for (int i = 0; i < dx1;) 
		{
			t1xp = 0; 
			t2xp = 0;

			if (t1x < t2x)
			{
			 	minx = t1x; 
			 	maxx = t2x; 
			}
			else
			{ 
				minx = t2x; 
				maxx = t1x;
			}

			while (i < dx1) 
			{
				i++;
				
				e1 += dy1;
				
				while (e1 >= dx1) 
				{
					e1 -= dx1;
					
					if (changed1) 
					{
						t1xp = signx1;
					}
					else
					{	
						goto next1;
					}          
				}

				if (changed1)
				{
					break;
				}
				else 
				{
					t1x += signx1;
				}
			}
			
			next1:

			while (1) 
			{
				e2 += dy2;

				while (e2 >= dx2) 
				{
					e2 -= dx2;

					if (changed2)
					{
						t2xp = signx2;
					}
					else
					{
						goto next2;
					}
				}

				if (changed2)
				{     
					break;
				}
				else
				{
					t2x += signx2;
				}
			}

			next2:
			
			if (minx > t1x)
			{
				minx = t1x;
			}

			if (minx > t2x) 
			{
				minx = t2x;
			}

			if (maxx < t1x) 
			{
				maxx = t1x;
			} 

			if (maxx < t2x)
			{ 
				maxx = t2x;
			}

			for (int x = minx; x <= maxx; x++)
			{
				if (x >= 0 && x < width && y >= 0 && y < height)
				{
					pixels[y * width + x] = shader
					(
						x, y,

						tx1, ty1,
						tx2, ty2,
						tx3, ty3,

						d1,
						d2,
						d3,

						this
					);
				}
			}
										 
			if (!changed1) 
			{
				t1x += signx1;
			}

			t1x += t1xp;

			if (!changed2)
			{
				t2x += signx2;
			}
			
			t2x += t2xp;
			
			y += 1;
			
			if (y == y2)
			{
				break;
			}
		}

		next:

		dx1 = (int)(x3 - x2); 
		dy1 = (int)(y3 - y2);

		if (dx1 < 0)
		{ 
			dx1 = -dx1; 

			signx1 = -1; 
		}
		else
		{
			signx1 = 1;
		}
		
		t1x = x2;

		if (dy1 > dx1) 
		{
			std::swap(dy1, dx1);

			changed1 = true;
		}
		else 
		{
			changed1 = false;
		}

		e1 = (int)(dx1 >> 1);

		for (int i = 0; i <= dx1; i++) 
		{
			t1xp = 0; 
			t2xp = 0;

			if (t1x < t2x)
			{ 
				minx = t1x; 
				maxx = t2x; 
			}
			else 
			{ 
				minx = t2x; 
				maxx = t1x; 
			}
			
			while (i < dx1) 
			{
				e1 += dy1;
			
				while (e1 >= dx1) 
				{
					e1 -= dx1;
					
					if (changed1) 
					{ 
						t1xp = signx1; 

						break; 
					}
					else
					{          
						goto next3;
					}
				}

				if (changed1) 
				{
					break;
				}
				else
				{ 	  
					t1x += signx1;
				};

				if (i < dx1) 
				{
					i++;
				}
			}

			next3:
			
			while (t2x != x3) 
			{
				e2 += dy2;

				while (e2 >= dx2) 
				{
					e2 -= dx2;

					if (changed2) 
					{
						t2xp = signx2;
					}
					else
					{
						goto next4;
					}
				}

				if (changed2)
				{
					break;
				}
				else
				{
					t2x += signx2;
				}
			}

			next4:

			if (minx > t1x) 
			{
				minx = t1x;
			} 

			if (minx > t2x) 
			{
				minx = t2x;
			}

			if (maxx < t1x) 
			{
				maxx = t1x;
			} 

			if (maxx < t2x) 
			{
				maxx = t2x;
			}

			for (int x = minx; x <= maxx; x++)
			{
				if (x >= 0 && x < width && y >= 0 && y < height)
				{
					pixels[y * width + x] = shader
					(
						x, y,

						tx1, ty1,
						tx2, ty2,
						tx3, ty3,

						d1,
						d2,
						d3,

						this
					);
				}
			}

			t1x += t1xp;
			t2x += t2xp;  										
			
			if (!changed1) 
			{
				t1x += signx1;
			}

			if (!changed2) 
			{
				t2x += signx2;
			}		
			
			y += 1;
			
			if (y > y3) 
			{
				return;
			}
		}
	}

	// The points in the simulation.

	std::vector<point*> points;

	// The constraints in the simulation.

	std::vector<constraint> constraints;

	// Make a random point.

	point* random_point()
	{
		return new point(randx(), randy());
	}

	double clw = 60;
	double clh = 30;

	// Add fabric.

	void fabric()
	{
		double hy = 60.0;

		double hx = width / 2.0 - (10.0 * clw / 2.0) + (10.0 / 2.0);

		for (int i = 0; i < clw; i++)
		{
			for (int j = 0; j < clh; j++)
			{
				points.push_back(new point(hx + (10.0 * i), j * 10.0 + hy));
			}

			points[i * clh]->locked = true;

			for (int j = 0; j < clh - 1; j++)
			{
				constraints.push_back(constraint(points[i * clh + j], points[i * clh + j + 1]));
			}
		}

		for (int i = 0; i < clw - 1; i++)
		{
			for (int j = 0; j < clh; j++)
			{
				constraints.push_back(constraint(points[i * clh + j], points[(i + 1) * clh + j]));
			}
		}
	}

	// Handle a keypress using the Boiler framework.

	void keydown(SDL_Event e) override
	{
		if (e.key.keysym.sym == SDLK_SPACE)
		{
			points.clear();

			constraints.clear();

			// New fabric.

			fabric();
		}
	}

	// Initialize the Boiler framework.

	void steam() override
	{
		// Seed the random number generator.

		srand((unsigned int)time(NULL));

		// Initialize the Boiler window.

		width = 800;
		height = 600;

		title = "Verlet integration (using Boiler)";

		// Add fabric to the simulation.

		fabric();
	}

	double* sx = nullptr;
	double* sy = nullptr;

	double* csx = nullptr;
	double* csy = nullptr;

	// Render a frame using the Boiler framework.

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		// Drag points with left click.

		if (mouse_l && sx == nullptr)
		{
			for (int i = 0; i < points.size(); i++)
			{
				if 
				(
					(mouse_x - points[i]->px) * (mouse_x - points[i]->px) +
					(mouse_y - points[i]->py) * (mouse_y - points[i]->py) 

					<= 

					5.0 * 5.0
				)
				{
					sx = &(points[i]->px);
					sy = &(points[i]->py);
				}
			}
		}
		else if (mouse_l && sx != nullptr)
		{
			*sx = mouse_x;
			*sy = mouse_y;
		}
		else
		{
			sx = nullptr;
			sy = nullptr;
		}

		// Update all points.

		for (int i = 0; i < points.size(); i++)
		{
			point* p = points[i];

			if (p->locked == false)
			{
				double vx = (p->px - p->opx) * 0.99;
				double vy = (p->py - p->opy) * 0.99;

				vx = clampm(vx, 50.0);
				vy = clampm(vy, 50.0);

				p->opx = p->px;
				p->opy = p->py;

				p->px += vx;
				p->py += vy;

				p->py += 0.2;
			}
		}

		// Constrain all points to the boundaries.

		for (int i = 0; i < points.size(); i++)
		{
			point* p = points[i];

			double vx = (p->px - p->opx) * 0.99;
			double vy = (p->py - p->opy) * 0.99;

			if (p->px > width)
			{
				p->px = width;

				p->opx = p->px + vx * 0.5;
			}
			else if (p->px < 0.0)
			{
				p->px = 0.0;

				p->opx = p->px + vx * 0.5;
			}

			if (p->py > height)
			{
				p->py = height;

				p->opy = p->py + vy * 0.5;
			}
			else if (p->py < 0.0)
			{
				p->py = 0.0;

				p->opy = p->py + vy * 0.5;
			}
		}

		// Apply forces of all constraints.

		for (int i = 0; i < constraints.size(); i++)
		{
			if (constraints[i].active)
			{
				constraints[i].do_constraint();
			}
		}

		// Texture the cloth.

		for (int i = 0; i < clw - 1; i++)
		{
			for (int j = 0; j < clh - 1; j++)
			{
				// Top left

				uv uv1 = uv(i / clw, j / clh);

				// Bottom left

				uv uv2 = uv(i / clw, (j + 1) / clh);

				// Top right

				uv uv3 = uv((i + 1) / clw, j / clh);

				shadetriangle
				(
					// Top left

					points[i * clh + j]->px,
					points[i * clh + j]->py,

					// Bottom left

					points[i * clh + j + 1]->px,
					points[i * clh + j + 1]->py,

					// Top right

					points[(i + 1) * clh + j]->px,
					points[(i + 1) * clh + j]->py,

					&uv1,
					&uv2,
					&uv3,

					my_shader
				);

				// Bottom left

				uv1 = uv(i / clw, (j + 1) / clh);

				// Bottom right

				uv2 = uv((i + 1) / clw, (j + 1) / clh);

				// Top right

				uv3 = uv((i + 1) / clw, j / clh);

				shadetriangle
				(
					// Bottom left

					points[i * clh + j + 1]->px,
					points[i * clh + j + 1]->py,

					// Bottom right

					points[(i + 1) * clh + j + 1]->px,
					points[(i + 1) * clh + j + 1]->py,

					// Top right

					points[(i + 1) * clh + j]->px,
					points[(i + 1) * clh + j]->py,	

					&uv1,
					&uv2,
					&uv3,

					my_shader
				);
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