# Boiler

Boiler is a graphics and rasterization framework built on top of SDL 2.

## Summary

This header file is a boilerplate header for making graphical projects using the SDL library. I created this header to help me begin SDL projects quickly and efficiently, so that I don't need to write boilerplate code every time.

This header is not meant for creating games in the traditional way of loading textures and drawing them directly to the screen. This header requires that you place each pixel yourself, so that you have more control over the graphics displayed on the screen.

Finally, this header requires an object to be used for each project. This means that you will need to overload functions and create child classes, but that is very simple. A sample project is included with this header to show you how to use it. Thank you!

## Usage

You can create a window that displays the default gradient using this simple code

```cpp
#include "boiler.h"

struct game: boiler
{	
	void steam() override
	{
		width = 800;
		height = 600;

		title = "Software renderer (using Boiler)";
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
```

It is extremely easy to draw shapes to the screen. Boiler currently supports drawing circles, triangles, lines, and rectangles.

## Building

To build any project that uses the Boiler framework, you must link the SDL 2 library. There are many tutorials that show you how to do this. For Clang, I use the following command

```bash
clang++ my_project.cpp -std=c++11 -lSDL2 -Ofast
```

## Demonstrations

Boiler comes with a few simple demonstrations, which can be found in the `/boiler/demos/` directory. The framework is extremely simple, looking at `boiler.h` should suffice for most applications.

## Made with Boiler

### Balls

Balls is a simple, inaccurate and fun toy for simulating the physics of rigid circular bodies. This simulation supports circle vs. circle collisions, along with circle vs. capsule collisions. It also supports momentum conservation, displacement and gravity. It is oddly satisfying to create hourglasses, slopes, marble runs and other physics demonstrations with this tool.

![Screenshot](https://github.com/CobaltXII/boiler/blob/master/img/s_balls_1.png "Balls collecting in a bowl")

### Raster

Raster is a software rasterizer. It is capable of rendering three-dimensional objects to the screen as pixels; this is called rasterization. Currently, this rasterizer supports rotation, scaling, translation, projection, and limited clipping. The clipping is very buggy, so objects must be rendered at a distance. Texturing is not supported, objects are colored with a color derived from their normals. Only flat shading is implemented.

![Screenshot](https://github.com/CobaltXII/boiler/blob/master/img/s_raster_1.png "Schizophrenic teapot")

### Normals

This is an extremely simple normal mapping implementation. This code takes an image (called a normal map) and uses the color values to generate a map of normals. The normals are then used with shading (diffuse) and combined with point lights can generate extremely realistic lighting effects. This program renders a few lights interacting with a normal mapped image in three-dimensional space.

![Screenshot](https://github.com/CobaltXII/boiler/blob/master/img/s_normals_3.png "Schizophrenic normal mapped coin")