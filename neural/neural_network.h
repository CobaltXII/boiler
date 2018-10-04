#pragma once

// A neural network with a variable (positive) and constant after created amount of input, hidden
// and output nodes. Uses the Sigmoid activation function.

struct neural_network
{
	unsigned int i_nodes;
	unsigned int h_nodes;
	unsigned int o_nodes;
};