#include "../boiler/boiler.h"

// Clamp a value to the range 0 to 255.

inline double clamprgb(double x)
{
	return std::max(0.0, std::min(255.0, x));
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

struct game: boiler
{	
	// Simple shader. Treats vertex data as color objects.

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

		// Use the weights to calculate the weighted color channel values.

		color* c1 = (color*)d1;
		color* c2 = (color*)d2;
		color* c3 = (color*)d3;

		double cr = w1 * (double)(c1->r) + w2 * (double)(c2->r) + w3 * (double)(c3->r); 
		double cg = w1 * (double)(c1->g) + w2 * (double)(c2->g) + w3 * (double)(c3->g); 
		double cb = w1 * (double)(c1->b) + w2 * (double)(c2->b) + w3 * (double)(c3->b); 

		// Average the color values.

		double a = w1 + w2 + w3;

		cr = cr / a;
		cg = cg / a;
		cb = cb / a;

		// Clamp and return.

		return rgb
		(
			clamprgb(cr),
			clamprgb(cg),
			clamprgb(cb)
		);
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

	double tx1;
	double ty1;

	double tx2;
	double ty2;

	double tx3;
	double ty3;

	double* sx = nullptr;
	double* sy = nullptr;

	color c1 = color(1.0, 0.0, 0.0);
	color c2 = color(0.0, 1.0, 0.0);
	color c3 = color(0.0, 0.0, 1.0);

	void steam() override
	{
		width = 800;
		height = 600;

		title = "Software renderer (using Boiler)";

		// Initialize random number generator.

		srand((unsigned int)time(NULL));

		// Initialize vertices with random values.

		tx1 = rand() % width;
		ty1 = rand() % height;

		tx2 = rand() % width;
		ty2 = rand() % height;

		tx3 = rand() % width;
		ty3 = rand() % height;
	}

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		// Make vertices draggable.

		if (mouse_l && sx == nullptr)
		{
			if 
			(
				(mouse_x - tx1) * (mouse_x - tx1) +
				(mouse_y - ty1) * (mouse_y - ty1)

				<=

				(7.0 * 7.0)
			)
			{
				sx = &tx1;
				sy = &ty1;
			}
			else if 
			(
				(mouse_x - tx2) * (mouse_x - tx2) +
				(mouse_y - ty2) * (mouse_y - ty2)

				<=

				(7.0 * 7.0)
			)
			{
				sx = &tx2;
				sy = &ty2;
			}
			else if 
			(
				(mouse_x - tx3) * (mouse_x - tx3) +
				(mouse_y - ty3) * (mouse_y - ty3)

				<=

				(7.0 * 7.0)
			)
			{
				sx = &tx3;
				sy = &ty3;
			}
		}
		else if (!mouse_l)
		{
			sx = nullptr;
			sy = nullptr;
		}
		else
		{
			*sx = mouse_x;
			*sy = mouse_y;
		}

		// Draw the triangle.

		shadetriangle
		(
			tx1, ty1,
			tx2, ty2,
			tx3, ty3,

			&c1,
			&c2,
			&c3,

			my_shader
		);

		// Draw circles around the vertices.

		circlergb(tx1, ty1, 7.0, rgb(255, 255, 255));
		circlergb(tx2, ty2, 7.0, rgb(255, 255, 255));
		circlergb(tx3, ty3, 7.0, rgb(255, 255, 255));
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