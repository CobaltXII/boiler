// Dynamic lighting? Awesome.

#include "../boiler/boiler.h"

#include <tuple>
#include <vector>
#include <utility>
#include <iostream>
#include <algorithm>

const int cw = 16;

// Edge access constants.

enum __edge
{
	_E_north,
	_E_south,
	_E_east,
	_E_west
};

// An edge.

struct edge_t
{
	double sx;
	double sy;

	double ex;
	double ey;
};

// A cell.

struct cell_t
{
	// This array holds the unique identifiers of each of the four edges of the cell (which is 
	// rectangular). The identifiers are unique only within the edge pool.

	int _Edge_id[4];

	// This array holds a boolean value for each of the four edges of the cell. The boolean value
	// reports whether the edge exists or not.

	bool _Edge_exist[4];

	// This boolean reports whether this cell exists (is visible and physical) or does not exist 
	// (is invisible and does not affect light).

	bool _Cell_exist = false;
};

// A light.

struct light_t
{
	double ox;
	double oy;

	double ir;
	double ig;
	double ib;
};

struct game: boiler
{	
	// Light shader.

	static inline Uint32 light_shader
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
		light_t& l_i = g->_Lights[g->_Current_Light];

		double dx = (px - l_i.ox);
		double dy = (py - l_i.oy);

		double l_dist = 
		(
			dx * dx +
			dy * dy
		);

		double inv_dist = 1.0 / l_dist;

		return rgb
		(
			mclamprgb(l_i.ir * inv_dist * 255.0),
			mclamprgb(l_i.ig * inv_dist * 255.0),
			mclamprgb(l_i.ib * inv_dist * 255.0)
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
		),

		Uint32* target
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
					target[y * width + x] = shader
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
					target[y * width + x] = shader
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

	cell_t* _World;

	unsigned int width_C;
	unsigned int height_C;

	std::vector<edge_t> _Edges;

	void make_edge_map(int sx, int sy, int rw, int rh)
	{
		_Edges.clear();

		for (int x = 0; x < rw; x++)
		{
			for (int y = 0; y < rh; y++)
			{
				for (int j = 0; j < 4; j++)
				{
					_World[(y + sy) * width_C + (x + sx)]._Edge_exist[j] = false;

					_World[(y + sy) * width_C + (x + sx)]._Edge_id[j] = 0;
				}
			}
		}

		for (int x = 1; x < rw - 1; x++)
		{
			for (int y = 1; y < rh - 1; y++)
			{
				int _Idx = (y + sy) * width_C + (x + sx);

				int _Idxn = (y + sy - 1) * width_C + (x + sx);
				int _Idxs = (y + sy + 1) * width_C + (x + sx);

				int _Idxw = (y + sy) * width_C + (x + sx - 1);
				int _Idxe = (y + sy) * width_C + (x + sx + 1);

				if (_World[_Idx]._Cell_exist)
				{
					// West side.

					if (!_World[_Idxw]._Cell_exist)
					{
						if (_World[_Idxn]._Edge_exist[_E_west])
						{
							_Edges[_World[_Idxn]._Edge_id[_E_west]].ey += cw;

							_World[_Idx]._Edge_id[_E_west] = _World[_Idxn]._Edge_id[_E_west];

							_World[_Idx]._Edge_exist[_E_west] = true;
						}
						else
						{
							edge_t _Edge;

							_Edge.sx = (sx + x) * cw;
							_Edge.sy = (sy + y) * cw;

							_Edge.ex = _Edge.sx;

							_Edge.ey = _Edge.sy + cw;

							int _Eid = _Edges.size();

							_Edges.push_back(_Edge);

							_World[_Idx]._Edge_id[_E_west] = _Eid;

							_World[_Idx]._Edge_exist[_E_west] = true;
						}
					}

					// East side.

					if (!_World[_Idxe]._Cell_exist)
					{
						if (_World[_Idxn]._Edge_exist[_E_east])
						{
							_Edges[_World[_Idxn]._Edge_id[_E_east]].ey += cw;

							_World[_Idx]._Edge_id[_E_east] = _World[_Idxn]._Edge_id[_E_east];

							_World[_Idx]._Edge_exist[_E_east] = true;
						}
						else
						{
							edge_t _Edge;

							_Edge.sx = (sx + x + 1) * cw;

							_Edge.sy = (sy + y) * cw;

							_Edge.ex = _Edge.sx;

							_Edge.ey = _Edge.sy + cw;

							int _Eid = _Edges.size();

							_Edges.push_back(_Edge);

							_World[_Idx]._Edge_id[_E_east] = _Eid;

							_World[_Idx]._Edge_exist[_E_east] = true;
						}
					}

					// North side.

					if (!_World[_Idxn]._Cell_exist)
					{
						if (_World[_Idxw]._Edge_exist[_E_north])
						{
							_Edges[_World[_Idxw]._Edge_id[_E_north]].ex += cw;

							_World[_Idx]._Edge_id[_E_north] = _World[_Idxw]._Edge_id[_E_north];

							_World[_Idx]._Edge_exist[_E_north] = true;
						}
						else
						{
							edge_t _Edge;

							_Edge.sx = (sx + x) * cw;
							_Edge.sy = (sy + y) * cw;

							_Edge.ex = _Edge.sx + cw;

							_Edge.ey = _Edge.sy;

							int _Eid = _Edges.size();

							_Edges.push_back(_Edge);

							_World[_Idx]._Edge_id[_E_north] = _Eid;

							_World[_Idx]._Edge_exist[_E_north] = true;
						}
					}

					// South side.

					if (!_World[_Idxs]._Cell_exist)
					{
						if (_World[_Idxw]._Edge_exist[_E_south])
						{
							_Edges[_World[_Idxw]._Edge_id[_E_south]].ex += cw;

							_World[_Idx]._Edge_id[_E_south] = _World[_Idxw]._Edge_id[_E_south];

							_World[_Idx]._Edge_exist[_E_south] = true;
						}
						else
						{
							edge_t _Edge;

							_Edge.sx = (sx + x) * cw;

							_Edge.sy = (sy + y + 1) * cw;

							_Edge.ex = _Edge.sx + cw;

							_Edge.ey = _Edge.sy;

							int _Eid = _Edges.size();

							_Edges.push_back(_Edge);

							_World[_Idx]._Edge_id[_E_south] = _Eid;

							_World[_Idx]._Edge_exist[_E_south] = true;
						}
					}
				}
			}
		}
	}

	std::vector<std::tuple<double, double, double>> _Visible;

	static bool angular_sort
	(
		const std::tuple<double, double, double>& t1,
		const std::tuple<double, double, double>& t2
	)
	{
		return std::get<0>(t1) < std::get<0>(t2);
	}

	void raycast_polygon(double ox, double oy, double rad)
	{
		for (int i = 0; i < _Edges.size(); i++)
		{
			edge_t& e1 = _Edges[i];

			for (int i = 0; i < 2; i++)
			{
				double rdx = (i == 0 ? e1.sx : e1.ex) - ox;
				double rdy = (i == 0 ? e1.sy : e1.ey) - oy;

				double bng = atan2(rdy, rdx);

				double ang = 0.0;

				for (int j = 0; j < 3; j++)
				{
					if (j == 0)
					{
						ang = bng - 0.0001;
					}
					else if (j == 1)
					{
						ang = bng;
					}
					else if (j == 2)
					{
						ang = bng + 0.0001;
					}

					rdx = rad * cos(ang);
					rdy = rad * sin(ang);

					double min_d = INFINITY;

					double min_px = 0.0;
					double min_py = 0.0;

					double min_ang = 0.0;

					bool ray_valid = false;

					for (int k = 0; k < _Edges.size(); k++)
					{
						edge_t& e2 = _Edges[k];

						double sdx = e2.ex - e2.sx;
						double sdy = e2.ey - e2.sy;

						if (abs(sdx - rdx) > 0.0 && abs(sdy - rdy) > 0.0)
						{
							double t2 = (rdx * (e2.sy - oy) + (rdy * (ox - e2.sx))) / (sdx * rdy - sdy * rdx);

							double t1 = (e2.sx + sdx * t2 - ox) / rdx;

							if (t1 > 0.0 && t2 >= 0.0 && t2 <= 1.0)
							{
								if (t1 < min_d)
								{
									min_d = t1;

									min_px = ox + rdx * t1;
									min_py = oy + rdy * t1;

									min_ang = atan2(min_py - oy, min_px - ox);

									ray_valid = true;
								}
							}
						}
					}

					if (ray_valid)
					{
						_Visible.push_back(std::tuple<double, double, double>(min_ang, min_px, min_py));						
					}
				}
			}
		}

		std::sort
		(
			_Visible.begin(),
			_Visible.end(),

			angular_sort
		);
	}

	std::vector<light_t> _Lights;

	// A buffer is allocated for each light. The buffer must be the size of the screen, and will 
	// be cleared every frame. It will then be filled with the light produced by it's 
	// corresponding light. At the end of every frame, the colors are blended and rendered.

	std::vector<Uint32*> _Light_Buffers;

	void steam() override
	{
		width = 800;
		height = 608;

		width_C = width / cw;
		height_C = height / cw;

		//f_No_Debug = SDL_TRUE;

		title = "Dynamic lighting (using Boiler)";

		// Allocate the world.

		_World = (cell_t*)malloc(width_C * height_C * sizeof(cell_t));

		// Add boundaries.

		for (int i = 1; i < width_C - 1; i++)
		{
			_World[width_C + i]._Cell_exist = true;

			_World[(height_C - 2) * width_C + i]._Cell_exist = true;
		}

		for (int i = 1; i < height_C - 1; i++)
		{
			_World[i * width_C + 1]._Cell_exist = true;

			_World[i * width_C + (width_C - 2)]._Cell_exist = true;
		}

		// Add lights.

		{
			light_t _Light;

			_Light.ir = 10000.0;

			_Lights.push_back(_Light);

			// Allocate buffer.

			_Light_Buffers.push_back((Uint32*)malloc(width * height * sizeof(Uint32)));
		}

		{
			light_t _Light;

			_Light.ig = 10000.0;

			_Lights.push_back(_Light);

			// Allocate buffer.

			_Light_Buffers.push_back((Uint32*)malloc(width * height * sizeof(Uint32)));
		}

		{
			light_t _Light;

			_Light.ib = 10000.0;

			_Lights.push_back(_Light);

			// Allocate buffer.

			_Light_Buffers.push_back((Uint32*)malloc(width * height * sizeof(Uint32)));
		}
	}

	int _Current_Light = 0;

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		// Toggle state.

		if (ml_pressed)
		{
			int cxi = mouse_x / cw;
			int cyi = mouse_y / cw;

			_World[cyi * width_C + cxi]._Cell_exist = !_World[cyi * width_C + cxi]._Cell_exist;
		}

		// Generate edge map.

		make_edge_map(0, 0, width_C, height_C);

		// Draw tile map.

		for (int x = 0; x < width / cw; x++)
		{
			for (int y = 0; y < height / cw; y++)
			{
				if (_World[y * width_C + x]._Cell_exist == true)
				{
					frectrgb(x * cw, y * cw, cw, cw, rgb(0, 0, 0));
				}
			}
		}

		// Spin lights.

		_Lights[0].ox = sin(iteration / 100.0) * 200.0 + h_width;
		_Lights[0].oy = cos(iteration / 100.0) * 200.0 + h_height;

		_Lights[1].ox = sin(iteration / 100.0 + mdegrad(360.0 / 3.0 * 1.0)) * 200.0 + h_width;
		_Lights[1].oy = cos(iteration / 100.0 + mdegrad(360.0 / 3.0 * 1.0)) * 200.0 + h_height;

		_Lights[2].ox = sin(iteration / 100.0 + mdegrad(360.0 / 3.0 * 2.0)) * 200.0 + h_width;
		_Lights[2].oy = cos(iteration / 100.0 + mdegrad(360.0 / 3.0 * 2.0)) * 200.0 + h_height;

		if (true)
		{
			for (_Current_Light = 0; _Current_Light < _Lights.size(); _Current_Light++)
			{
				memset((void*)_Light_Buffers[_Current_Light], 0, width * height * sizeof(Uint32));

				double orig_x = _Lights[_Current_Light].ox;
				double orig_y = _Lights[_Current_Light].oy;

				_Visible.clear();

				raycast_polygon(orig_x, orig_y, 1000.0);
				raycast_polygon(orig_x, orig_y, 1000.0);

				if (_Visible.size() > 1)
				{
					// Draw light.

					for (int i = 0; i < _Visible.size() - 1; i++)
					{
						shadetriangle
						(
							(int)orig_x,
							(int)orig_y,

							std::get<1>(_Visible[i]),
							std::get<2>(_Visible[i]),

							std::get<1>(_Visible[i + 1]),
							std::get<2>(_Visible[i + 1]),

							nullptr,
							nullptr,
							nullptr,

							light_shader,

							_Light_Buffers[_Current_Light]
						);
					}

					shadetriangle
					(
						(int)orig_x,
						(int)orig_y,

						std::get<1>(_Visible[_Visible.size() - 1]),
						std::get<2>(_Visible[_Visible.size() - 1]),

						std::get<1>(_Visible[0]),
						std::get<2>(_Visible[0]),

						nullptr,
						nullptr,
						nullptr,

						light_shader,

						_Light_Buffers[_Current_Light]
					);
				}
			}

			for (int x = 0; x < width; x++)
			{
				for (int y = 0; y < height; y++)
				{
					Uint32 bcol = pixels[y * width + x];

					int ir = mgetr(bcol);
					int ig = mgetg(bcol);
					int ib = mgetb(bcol);

					for (int i = 0; i < _Lights.size(); i++)
					{
						ir += mgetr(_Light_Buffers[i][y * width + x]);
						ig += mgetg(_Light_Buffers[i][y * width + x]);
						ib += mgetb(_Light_Buffers[i][y * width + x]);
					}

					pixels[y * width + x] = rgb
					(
						mclamprgb(ir),
						mclamprgb(ig),
						mclamprgb(ib)
					);
				}
			}
		}

		// Draw edge map.

		for (int i = 0; i < _Edges.size(); i++)
		{
			edge_t& _edge = _Edges[i];

			linergb(_edge.sx, _edge.sy, _edge.ex, _edge.ey, rgb(255, 255, 255));
		}

		for (int i = 0; i < _Edges.size(); i++)
		{
			edge_t& _edge = _Edges[i];

			fcirclergb(_edge.sx, _edge.sy, 3, rgb(255, 255, 255));
			fcirclergb(_edge.ex, _edge.ey, 3, rgb(255, 255, 255));
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

	// Free the world.

	free(demo._World);

	// Free buffers.

	for (int i = 0; i < demo._Lights.size(); i++)
	{
		free(demo._Light_Buffers[i]);
	}

	return 0;
}