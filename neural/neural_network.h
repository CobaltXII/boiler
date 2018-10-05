#pragma once

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
};