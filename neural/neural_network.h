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
};