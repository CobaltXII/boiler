/*

Train a neural network to recognize handwritten digits using the MNIST dataset.

	# Train a new brain saved at <brain-path> with 10 iterations

	./train -o <brain-path> 10

	# Continue training the brain at <brain-path> with 10 more iterations

	./train -r <brain-path> 10

*/

#define BOIL_USE_STB_IMAGE

#include "../boiler/boiler.h"

#include <dirent.h>

#include <random>
#include <vector>
#include <utility>
#include <iostream>

#include "../neural/neural_matrix.h"
#include "../neural/neural_network.h"

std::vector<std::string> enum_files(std::string _Path)
{
	std::vector<std::string> _Out;

	DIR* dir_parent = opendir(_Path.c_str());

	dirent* dir_entry;

	while ((dir_entry = readdir(dir_parent)) != NULL)
	{
		_Out.push_back(dir_entry->d_name);
	}

	closedir(dir_parent);

	_Out.erase(_Out.begin());
	_Out.erase(_Out.begin());

	return _Out;
}

typedef std::vector<double> mnist_t;

std::vector<mnist_t> fetch_glyphs(std::string _Parent, std::vector<std::string> _Paths)
{
	std::vector<mnist_t> _Out;

	int dummy_w;
	int dummy_h;

	for (int i = 0; i < _Paths.size(); i++)
	{
		Uint32* c_glyph = loadimg(_Parent + _Paths[i], dummy_w, dummy_h);

		mnist_t My_Glorious_Glyph;

		for (int v = 0; v < 28; v++)
		{
			for (int u = 0; u < 28; u++)
			{
				My_Glorious_Glyph.push_back((double)(getr(c_glyph[v * 28 + u])) / 255.0 * 2.0 - 1.0);
			}
		}

		free(c_glyph);

		_Out.push_back(My_Glorious_Glyph);
	}

	return _Out;
}

std::vector<double> dess_output(int _Which)
{
	std::vector<double> _Dess;

	for (int i = 0; i < 10; i++)
	{
		if (i == _Which)
		{
			_Dess.push_back(1.0);
		}
		else
		{
			_Dess.push_back(0.0);
		}
	}

	return _Dess;
}

void nuke(std::string _Note)
{
	std::cout << _Note << std::endl;

	exit(1);
}

int main(int argc, char** argv)
{
	if (argc != 4)
	{
		nuke("Invalid usage.");
	}

	int _Usage;

	if (argv[1] == "-o")
	{
		_Usage = 1;
	}
	else if (argv[1] == "-r")
	{
		_Usage = 2;
	}
	else
	{
		nuke("Invalid usage.");
	}

	if (_Usage == 2)
	{
		std::ifstream _Exist = std::ifstream(argv[2]);

		if (_Exist.good() == false)
		{
			nuke("Invalid path.");
		}
	}

	std::ofstream _File = std::ofstream(argv[2]);

	std::vector<std::string> train_names_0 = enum_files("mnist_png/training/0");
	std::vector<std::string> train_names_1 = enum_files("mnist_png/training/1");
	std::vector<std::string> train_names_2 = enum_files("mnist_png/training/2");
	std::vector<std::string> train_names_3 = enum_files("mnist_png/training/3");
	std::vector<std::string> train_names_4 = enum_files("mnist_png/training/4");
	std::vector<std::string> train_names_5 = enum_files("mnist_png/training/5");
	std::vector<std::string> train_names_6 = enum_files("mnist_png/training/6");
	std::vector<std::string> train_names_7 = enum_files("mnist_png/training/7");
	std::vector<std::string> train_names_8 = enum_files("mnist_png/training/8");
	std::vector<std::string> train_names_9 = enum_files("mnist_png/training/9");

	std::vector<mnist_t> train_glyphs_0 = fetch_glyphs("mnist_png/training/0/", train_names_0);
	std::vector<mnist_t> train_glyphs_1 = fetch_glyphs("mnist_png/training/1/", train_names_1);
	std::vector<mnist_t> train_glyphs_2 = fetch_glyphs("mnist_png/training/2/", train_names_2);
	std::vector<mnist_t> train_glyphs_3 = fetch_glyphs("mnist_png/training/3/", train_names_3);
	std::vector<mnist_t> train_glyphs_4 = fetch_glyphs("mnist_png/training/4/", train_names_4);
	std::vector<mnist_t> train_glyphs_5 = fetch_glyphs("mnist_png/training/5/", train_names_5);
	std::vector<mnist_t> train_glyphs_6 = fetch_glyphs("mnist_png/training/6/", train_names_6);
	std::vector<mnist_t> train_glyphs_7 = fetch_glyphs("mnist_png/training/7/", train_names_7);
	std::vector<mnist_t> train_glyphs_8 = fetch_glyphs("mnist_png/training/8/", train_names_8);
	std::vector<mnist_t> train_glyphs_9 = fetch_glyphs("mnist_png/training/9/", train_names_9);

	std::vector<double> dess_output_0 = dess_output(0);
	std::vector<double> dess_output_1 = dess_output(1);
	std::vector<double> dess_output_2 = dess_output(2);
	std::vector<double> dess_output_3 = dess_output(3);
	std::vector<double> dess_output_4 = dess_output(4);
	std::vector<double> dess_output_5 = dess_output(5);
	std::vector<double> dess_output_6 = dess_output(6);
	std::vector<double> dess_output_7 = dess_output(7);
	std::vector<double> dess_output_8 = dess_output(8);
	std::vector<double> dess_output_9 = dess_output(9);
}