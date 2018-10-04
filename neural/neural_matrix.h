#pragma once

std::random_device r_device;

// A matrix with a variable (positive) number of rows and columns.

struct matrix
{
	unsigned int r_C;
	unsigned int c_C;

	// Remember, this vector contains a vector of rows. The rows contain values. When we access 
	// this vector, the syntax is u_M[row][column].

	std::vector<std::vector<double>> u_M;

	matrix(unsigned int _r_C, unsigned int _c_C)
	{
		r_C = _r_C;
		c_C = _c_C;

		// Initialize empty matrix.

		for (int i = 0; i < r_C; i++)
		{
			std::vector<double> _R;

			for (int j = 0; j < c_C; j++)
			{
				_R.push_back(0.0);
			}

			u_M.push_back(_R);
		}
	}

	// Multiply each value in the matrix by a scalar value.

	void scale(double _S)
	{
		for (int i = 0; i < r_C; i++)
		{
			for (int j = 0; j < c_C; j++)
			{
				u_M[i][j] *= _S;
			}
		}
	}

	// Calculate the Hadamard product of two matrices with the same dimensions.

	void scale(matrix _M)
	{
		for (int i = 0; i < r_C; i++)
		{
			for (int j = 0; j < c_C; j++)
			{
				u_M[i][j] *= _M.u_M[i][j];
			}
		}
	}
};