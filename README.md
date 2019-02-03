# Boiler

Boiler is a graphics and rasterization framework built on top of SDL2.

## Summary

Boiler is a small, header-only graphics, audio and rasterization library for creating cross-platform applications based on SDL2. Boiler was created to provide a simple, fast and extensible interface to the SDL2 library.

Boiler is oriented around per-pixel rasterization. It does not use any SDL2 graphics routines, such as `SDL_RenderDrawLine` or `SDL_RenderCopy`. Instead, it implements rasterization functions itself, to provide an extensible and flexible graphics interface.

## Usage

Boiler is an object-oriented library. It provides a base class with all the implemented drawing routines and class members. The standard header also defines many global functions for convenience. 

To set up Boiler, you must create your own class that inherits from the generic `boiler` class. You may then override `steam` to initialize the underlying SDL2 window. You should also override `draw`, and optionally `keydown` and `keyup` and other functions as well. 

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

## License

The Boiler framework and all other included projects are licensed under the MIT License.
