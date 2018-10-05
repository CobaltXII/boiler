#pragma once

// A neural network with a variable (positive) and constant after created amount of input, hidden
// and output nodes.

struct neural_network
{
	unsigned int i_nodes;
	unsigned int h_nodes;
	unsigned int o_nodes;

	matrix ih_w;
	matrix ho_w;
};