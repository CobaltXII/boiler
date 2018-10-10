# Boiler

Boiler is a graphics and rasterization framework built on top of SDL2.

## Summary

Boiler is a small, header-only graphics and rasterization library for creating cross-platform applications based on SDL2. Boiler was created to provide a simple, fast and extensible interface to the SDL2 library.

Boiler is oriented around per-pixel rasterization. It does not use any SDL2 graphics routines, such as `SDL_RenderDrawLine` or `SDL_RenderCopy`. Instead, it implements rasterization functions itself, to provide an extensible and flexible graphics interface.

## Usage

Boiler is an object-oriented library. It provides a base class with all the implemented drawing routines and class members. The standard header also defines many global functions for convenience. 

To set up Boiler, you must create your own class that inherits from the generic `boiler` class. You may then override `steam` to initialize the underlying SDL2 window. You should also override `draw`, and optionally `keydown` and `keyup` as well. 

To use Boiler, you must create a default instance of your child class which inherits from `boiler`. You then call `make`, which will return a non-zero value if it fails. After that, you should call `engine`, which will execute until the program exits. After calling `engine`, calling `sweep` is good practise to ensure all memory is freed.

You can find a few well commented and detailed (yet simple) demonstrations using the Boiler framework in the `demos` directory. These demonstrations showcase the simplicity of the framework.

You can create a window that displays the default gradient sample using this simple code

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

## Building

To build any project that uses the Boiler framework, you must link the SDL2 library. There are many tutorials that show you how to do this. For Clang or GCC, the following command should suffice

```bash
clang++ my_project.cpp -std=c++11 -lSDL2 -Ofast
```

**To Mojave users:** Since the release of Mojave, the Boiler framework has a few bugs which are associated with the SDL2 library. These bugs cause errors in many OpenGL frameworks (including Boiler), so you may experience them in your project. The most common bug is the one that causes the window to appear black until it is moved (by the user, drag it using the title bar). This will be fixed in SDL 2.0.9, which should be released soon.

## Other

**If all you are looking for is the single Boiler header**, and not the associated projects, then do not clone the repository! Most of the repository is not the framework itself, but projects that I have created which utilize the framework. You can find a direct link to the header [here](https://raw.githubusercontent.com/CobaltXII/boiler/master/boiler/boiler.h "Direct download"). 

You may have noticed that there are many directories in this repository, and they do not seem relevant to the Boiler framework. These sub-repositories, to use my word for them, are projects that I have made myself (for fun) that utilize the Boiler framework. I encourage you to take a look at these projects, they show how Boiler makes it extremely easy to render graphics quickly and cleanly.

If you ever use any concepts or code from any of the files in this repository, I would greatly appreciate it if you could credit (or link) this repository. Notifying me that you have used this code would also be appreciated. Thank you!

### Balls

Balls is a simple, inaccurate and fun toy for simulating the physics of rigid circular bodies. The simulation supports circle vs. circle collisions, along with circle vs. capsule collisions. It also supports momentum conservation, displacement and gravity. It is oddly satisfying to create hourglasses, slopes, marble runs and other physics demonstrations with this tool.

### Raster

Raster is a software rasterizer. It is capable of rendering three-dimensional objects to the screen as pixels; this is called rasterization. Currently, this rasterizer supports rotation, scaling, translation, projection, and limited clipping. The clipping is very buggy, so objects must be rendered at a distance. Texturing is not supported, objects are colored with a color derived from their normals. Only flat shading is implemented.

### Normals

This is an extremely simple normal mapping implementation. This code takes an image (called a normal map) and uses the color values to generate a map of normals. A diffuse shading implementation uses the normal map and a list of point lights to generate extremely realistic lighting effects. This program renders a few lights interacting with a normal mapped image in three-dimensional space.

### Barycentric

Barycentric is a triangle renderer. It renders triangles using a hard-coded shader, which can manipulate the triangle in any way. The renderer uses barycentric coordinates to 'weigh' the vertex data of the triangle, and then uses those weights to fetch texture information or generate a color value. The renderer is moderately fast, and I hope to use it in my future projects. Barycentric currently has color and texture interpolation support.

### Additive

Additive is an additive blending demonstration. It features three circles, which are red, green and blue. The circles pulse, spin, and swap positions at a fast pace. Additive blending is a form of color blending, which closely mimics the effect produced by blending physical lights. Additive blending can be used in many applications, such as dynamic lighting, realistic particle effects, glowing sprites, and rendering.

### Sierpiński

Sierpiński is a simple program to render the Sierpiński triangle fractal. I thought of the algorithm to render the fractal myself, but I am sure that someone else has thought of a similar (if not identical) algorithm before. No recursive functions are used, just a vector of triangles that mutate on demand. I find watching the Sierpiński triangle progress very satisfying, and the resulting pattern is beautiful.

### Cloth

Cloth is a cloth simulation implemented using Verlet integration. The cloth simulation is made up of tiny point masses arranged in a net or grid pattern, which are linked together with rigid constraints. The constraints try hard to keep the two points they are tethered to a certain distance from each other, but sometimes external forces are too great; the constraints break. Currently, this project supports untextured, rippable cloth, and textured, unrippable cloth.

### Dither

Dither is a tiny dithering library. It supports black and white thresholding, random dithering, Gaussian dithering, and ordered dithering. It also supports splicing color images into their one-dimensional channels, and joining grayscale images to create color images. It includes Bayer matrices of varying sizes, cluster dot matrices, horizontal line matrices and vertical line matrices. Gamma correction is also implemented. It can be used for 1-bit grayscale dithering, or 3-bit color dithering.

### Neural

Neural is a tiny neural network library. It supports creating neural networks with any amount of input, hidden and output nodes. It supports forward propagation using Sigmoid activation, and back propagation using gradient descent. The library also implements it's own matrix library, which has many common functions and is useable for other applications as well. Currently, a 1-dimensional XOR solver and a 3-dimensional RGB solver have been created with this library.

## Dependencies

The Boiler framework by default does not have any dependancies. However, to unlock certain features such as the `loadimg` function, you must define `BOIL_USE_STB_IMAGE`, which includes the `stb_image` header file. Thanks to Sean T. Barrett and contributors for creating that wonderful API.

## License

The Boiler framework and all other included projects are licensed under the [GNU General Public License v3.0](https://github.com/CobaltXII/boiler/blob/master/LICENSE "GNU General Public License v3.0"). A short paragraph provided by Github briefly summarizes the license

> Permissions of this strong copyleft license are conditioned on making available complete source code of licensed works and modifications, which include larger works using a licensed work, under the same license. Copyright and license notices must be preserved. Contributors provide an express grant of patent rights. This is not legal advice.
