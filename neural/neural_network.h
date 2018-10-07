/*

Neural network library. Based off of Daniel Shiffman's tutorials.

	https://www.youtube.com/watch?v=qWK7yW8oS0I
	https://www.youtube.com/watch?v=MPmLWsHzPlU
	https://www.youtube.com/watch?v=QJoa0JYaX1I

	...

*/

#pragma once

#include <sstream>

// The Sigmoid activation function.

inline double sigmoid(double _Arg)
{
	return 1.0 / (1.0 + std::exp(-_Arg));
}

inline double deriv_sigmoid(double _Sig_Arg)
{
	return _Sig_Arg * (1.0 - _Sig_Arg);
}

// A neural network with a variable (positive) and constant after created amount of input, hidden
// and output nodes.

struct neural_network
{
	unsigned int i_nodes;
	unsigned int h_nodes;
	unsigned int o_nodes;

	matrix ih_w;
	matrix ho_w;

	matrix h_b;
	matrix o_b;

	double l_R; 

	neural_network
	(
		unsigned int _I,
		unsigned int _H,
		unsigned int _O
	)
	{
		i_nodes = _I;
		h_nodes = _H;
		o_nodes = _O;

		ih_w = matrix(_H, _I);
		ho_w = matrix(_O, _H);

		ih_w.randomize();
		ho_w.randomize();

		h_b = matrix(_H, 1);
		o_b = matrix(_O, 1);

		h_b.randomize();
		o_b.randomize();

		l_R = 0.1;
	}

	neural_network
	(
		unsigned int _I,
		unsigned int _H,
		unsigned int _O,

		double _l_R
	)
	{
		i_nodes = _I;
		h_nodes = _H;
		o_nodes = _O;

		ih_w = matrix(_H, _I);
		ho_w = matrix(_O, _H);

		ih_w.randomize();
		ho_w.randomize();

		h_b = matrix(_H, 1);
		o_b = matrix(_O, 1);

		h_b.randomize();
		o_b.randomize();

		l_R = _l_R;
	}

	std::vector<double> feed_forward(std::vector<double>& _In)
	{
		// Convert our inputs into a matrix, so that it can be used as an operand for all matrix
		// operations. This makes the code more convenient, legible and faster.

		matrix _Im = matrix(_In);

		// Use the Sigmoid activation function to calculate the outputs of the hidden layer of 
		// this neural network.

		matrix _Mh = matrix::multiply(ih_w, _Im).add(h_b).map(sigmoid);

		// Use the Sigmoid activation function to calculate the outputs of the output layer of 
		// this neural network, using the outputs of the hidden layer as input.

		matrix _Mo = matrix::multiply(ho_w, _Mh).add(o_b).map(sigmoid);

		// Convert to a vector, and return.

		return _Mo.vector();
	}

	void train(std::vector<double>& _In, std::vector<double>& _Target)
	{
		// Feed the inputs through the neural network to find the current result. This result will
		// be compared to the target result, and the weights and biases of the neural network will
		// be adjusted so that the next result will (hopefully) be closer to the target result.

		matrix _Im = matrix(_In);

		matrix _Mh = matrix::multiply(ih_w, _Im).add(h_b).map(sigmoid);

		matrix _Om = matrix::multiply(ho_w, _Mh).add(o_b).map(sigmoid);

		// Convert the targets to a matrix.

		matrix _Tm = matrix(_Target);

		// Calculate the error.

		matrix _Oem = matrix::subtract(_Tm, _Om);

		// Calculate the gradient.

		matrix _Gm = matrix::map(_Om, deriv_sigmoid).scale(_Oem).scale(l_R);

		// Calculate the hidden to output deltas.

		matrix _Mh_t = matrix::transpose(_Mh);

		matrix _Who_d = matrix::multiply(_Gm, _Mh_t);

		// Adjust the hidden to output weights by their corresponding deltas.

		ho_w.add(_Who_d);

		// Adjust the output bias by the gradients (it's deltas).

		o_b.add(_Gm);

		// Calculate the hidden layer error.

		matrix _Ho_wt = matrix::transpose(ho_w);

		matrix _Hem = matrix::multiply(_Ho_wt, _Oem);

		// Calculate the hidden gradient.

		matrix _Hgm = matrix::map(_Mh, deriv_sigmoid).scale(_Hem).scale(l_R);

		// Calculate the input to hidden deltas.

		matrix _Im_t = matrix::transpose(_Im);

    	matrix _Wih_d = matrix::multiply(_Hgm, _Im_t);

    	// Adjust the input to hidden weights by their corresponding deltas.

    	ih_w.add(_Wih_d);

    	// Adjust the hidden bias by the hidden gradients (it's deltas).

		h_b.add(_Hgm);
	}

	// Convert to a string.

	std::string stringify()
	{
		std::stringstream _S;

		_S << i_nodes << std::endl;
		_S << h_nodes << std::endl;
		_S << o_nodes << std::endl;

		for (int i = 0; i < h_nodes; i++)
		{
			for (int j = 0; j < i_nodes; j++)
			{
				_S << ih_w.u_M[i][j] << std::endl;
			}
		}

		for (int i = 0; i < o_nodes; i++)
		{
			for (int j = 0; j < h_nodes; j++)
			{
				_S << ho_w.u_M[i][j] << std::endl;
			}
		}

		for (int i = 0; i < h_nodes; i++)
		{
			_S << h_b.u_M[i][0] << std::endl;
		}

		for (int i = 0; i < o_nodes; i++)
		{
			_S << o_b.u_M[i][0] << std::endl;
		}

		_S << l_R << std::endl;

		return _S.str();
	}

	// Create from a string.

	static neural_network parse(std::string _In)
	{
		std::stringstream _Str = std::stringstream(_In);

		std::vector<std::string> _Lines;

		std::string _Line;

		while (std::getline(_Str, _Line))
		{
			_Lines.push_back(_Line);
		}

		unsigned int _I_Nodes = std::stoi(_Lines[0]);
		unsigned int _H_Nodes = std::stoi(_Lines[1]);
		unsigned int _O_Nodes = std::stoi(_Lines[2]);

		_Lines.erase(_Lines.begin());
		_Lines.erase(_Lines.begin());
		_Lines.erase(_Lines.begin());

		matrix _ih_w = matrix(_H_Nodes, _I_Nodes);
		matrix _ho_w = matrix(_O_Nodes, _H_Nodes);

		matrix _h_b = matrix(_H_Nodes, 1);
		matrix _o_b = matrix(_O_Nodes, 1);

		for (int i = 0; i < _H_Nodes; i++)
		{
			for (int j = 0; j < _I_Nodes; j++)
			{
				_ih_w.u_M[i][j] = std::stod(_Lines[(i * _I_Nodes) + j]);
			}
		}

		for (int i = 0; i < _O_Nodes; i++)
		{
			for (int j = 0; j < _H_Nodes; j++)
			{
				_ho_w.u_M[i][j] = std::stod(_Lines[(_H_Nodes * _I_Nodes) + (i * _H_Nodes) + j]);
			}
		}

		for (int i = 0; i < _H_Nodes; i++)
		{
			_h_b.u_M[i][0] = std::stod(_Lines[(_H_Nodes * _I_Nodes) + (_O_Nodes * _H_Nodes) + i]);
		}

		for (int i = 0; i < _O_Nodes; i++)
		{
			_o_b.u_M[i][0] = std::stod(_Lines[(_H_Nodes * _I_Nodes) + (_O_Nodes * _H_Nodes) + _H_Nodes + i]);
		}

		double _l_R = std::stod(_Lines[_Lines.size() - 1]);

		neural_network _Network = neural_network(_I_Nodes, _H_Nodes, _O_Nodes, _l_R);

		_Network.ih_w = _ih_w;
		_Network.ho_w = _ho_w;

		_Network.h_b = _h_b;
		_Network.o_b = _o_b;

		return _Network;
	}

	// Create from a file.

	static neural_network load(std::string _Path)
	{
		std::ifstream _In_File = std::ifstream(_Path);
	
		std::stringstream _Buffer;

		_Buffer << _In_File.rdbuf();

		return parse(_Buffer.str());
	}
};