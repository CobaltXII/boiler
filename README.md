# Boiler

Boiler is a graphics and rasterization framework built on top of SDL2.

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

To build any project that uses the Boiler framework, you must link the SDL2 library. There are many tutorials that show you how to do this. For Clang, I use the following command

```bash
clang++ my_project.cpp -std=c++11 -lSDL2 -Ofast
```

## Demonstrations

Boiler comes with a few simple demonstrations, which can be found in the `boiler/` directory. The framework is extremely simple, looking at `boiler.h` should suffice for most applications.

## Made with Boiler

### Balls

Balls is a simple, inaccurate and fun toy for simulating the physics of rigid circular bodies. This simulation supports circle vs. circle collisions, along with circle vs. capsule collisions. It also supports momentum conservation, displacement and gravity. It is oddly satisfying to create hourglasses, slopes, marble runs and other physics demonstrations with this tool.

![Screenshot](https://github.com/CobaltXII/boiler/blob/master/img/s_balls_2.png "Balls on slopes")

### Raster

Raster is a software rasterizer. It is capable of rendering three-dimensional objects to the screen as pixels; this is called rasterization. Currently, this rasterizer supports rotation, scaling, translation, projection, and limited clipping. The clipping is very buggy, so objects must be rendered at a distance. Texturing is not supported, objects are colored with a color derived from their normals. Only flat shading is implemented.

![Screenshot](https://github.com/CobaltXII/boiler/blob/master/img/s_raster_4.png "Stanford bunny")

### Normals

This is an extremely simple normal mapping implementation. This code takes an image (called a normal map) and uses the color values to generate a map of normals. The normals are then used with shading (diffuse) and combined with point lights can generate extremely realistic lighting effects. This program renders a few lights interacting with a normal mapped image in three-dimensional space.

![Screenshot](https://github.com/CobaltXII/boiler/blob/master/img/s_normals_3.png "Schizophrenic normal mapped coin")

### Barycentric

Barycentric is a triangle renderer. It renders triangles using a hard-coded shader, which can manipulate the triangle in any way. The renderer uses barycentric coordinates to 'weigh' the vertex data of the triangle, and then uses those weights to fetch texture information or generate a color value. The renderer is moderately fast, and I hope to use it in my future projects. Barycentric currently has color and texture interpolation support.

![Screenshot](https://github.com/CobaltXII/boiler/blob/master/img/s_barycentric_1.png "Colorful triangle")

### Additive

Additive is an additive blending demonstration. It features three circles, which are red, green and blue. The circles pulse, spin, and swap positions at a fast pace. Additive blending is a form of color blending, which closely mimics the effect produced by blending physical lights. Additive blending can be used in many applications, such as dynamic lighting, realistic particle effects, glowing sprites, and rendering.

![Screenshot](https://github.com/CobaltXII/boiler/blob/master/img/s_additive_1.png "Additive blending")

### Sierpiński

Sierpiński is a simple program to render the Sierpiński triangle fractal. I thought of the algorithm to render the fractal myself, but I am sure that someone else has thought of a similar (if not identical) algorithm before. No recursive functions are used, just a vector of triangles that mutate on demand. I find watching the Sierpiński triangle progress very satisfying, and the resulting pattern is beautiful.

![Screenshot](https://github.com/CobaltXII/boiler/blob/master/img/s_sierpinski_1.png "Recursive triforce (Twister#6074)")

### Cloth

Cloth is a cloth simulation using Verlet integration. The cloth simulation is made up of tiny point masses arranged in a net or grid pattern, which are linked together with rigid constraints. The constraints try hard to keep the two points they are tethered to a certain distance from each other, but sometimes external forces are too great; the constraints pop. Currently, this project supports untextured, rippable cloth, and textured, unrippable cloth.

![Screenshot](https://github.com/CobaltXII/boiler/blob/master/img/s_cloth_2.png "Canadian flag")
