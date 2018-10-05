# Boiler

Boiler is a graphics and rasterization framework built on top of SDL2.

## Summary

Boiler is a small, header-only graphics and rasterization library for creating cross-platform applications based on SDL2. Boiler was created to provide a simple, fast and extensible interface to the SDL2 library.

Boiler is oriented around per-pixel rasterization. It does not use any SDL2 graphics routines, such as `SDL_RenderDrawLine` or `SDL_RenderCopy`. Instead, it implements rasterization functions itself, to provide an extensible and flexible graphics interface.

## Usage

Boiler is an object-oriented library. It provides a base class with all the implemented drawing routines and class members. The standard header also defines many global functions for convenience. 

To set up Boiler, you must create your own class that inherits from the generic `boiler` class. You may then override `steam` to initialize the underlying SDL2 window. You should also override `draw`, and optionally `keydown` and `keyup` as well. 

To use Boiler, you must create a default instance of your child class which inherits from `boiler`. You then call `make`, which will return a non-zero value if it fails. After that, you should call `engine`, which will execute until the program exits. After calling `engine`, calling `sweep` is good practise to ensure all memory is freed.

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

## Other

**If all you are looking for is the single Boiler header**, and not the associated projects, then do not clone the repository! Most of the repository is not the framework itself, but projects that I have created which utilize the framework. You can find a direct link to the header [here](https://raw.githubusercontent.com/CobaltXII/boiler/master/boiler/boiler.h "Direct download"). 

You may have noticed that there are many directories in this repository, and they do not seem relevant to the Boiler framework. These sub-repositories, to use my word for them, are projects that I have made myself (for fun) that utilize the Boiler framework. I encourage you to take a look at these projects, they show how Boiler makes it extremely easy to render graphics quickly and cleanly.

If you ever use any concepts or code from any of the files in this repository, I would greatly appreciate it if you could credit (or link) this repository. Notifying me that you have used this code would also be appreciated. Thank you!

## License

The Boiler framework and all other included projects are licensed under the [GNU General Public License v3.0](https://github.com/CobaltXII/boiler/blob/master/LICENSE "GNU General Public License v3.0"). A short paragraph provided by Github briefly summarizes the license

> Permissions of this strong copyleft license are conditioned on making available complete source code of licensed works and modifications, which include larger works using a licensed work, under the same license. Copyright and license notices must be preserved. Contributors provide an express grant of patent rights. This is not legal advice.
