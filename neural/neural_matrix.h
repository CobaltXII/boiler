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

	// Calculate the matrix product of two matrices.

	static matrix multiply(matrix _A, matrix _B)
	{
		if (_A.c_C != _B.r_C)
		{
			throw std::runtime_error("Column count of _A must match row count of _B.");
		}

		matrix _O = matrix(_A.r_C, _B.c_C);

		for (int i = 0; i < _O.r_C; i++)
		{
			for (int j = 0; j < _O.c_C; j++)
			{
				double _S = 0.0;

				for (int k = 0; k < _A.c_C; k++)
				{
					_S += _A.u_M[i][k] * _B.u_M[k][j];
				}

				_O.u_M[i][j] = _S;
			}
		}

		return _O;
	}

	// Transpose a matrix.

	static matrix transpose(matrix _M)
	{
		matrix _O = matrix(_M.c_C, _M.r_C);

		for (int i = 0; i < _M.r_C; i++)
		{
			for (int j = 0; j < _M.c_C; j++)
			{
				_O.u_M[j][i] = _M.u_M[i][j];
			}
		}

		return _O;
	}

	// Add a scalar value to each value in the matrix.

	void add(double _R)
	{
		for (int i = 0; i < r_C; i++)
		{
			for (int j = 0; j < c_C; j++)
			{
				u_M[i][j] += _R;
			}
		}
	}

	// Add two matrices of the same dimensions together, element-wise.

	void add(matrix _M)
	{
		for (int i = 0; i < r_C; i++)
		{
			for (int j = 0; j < c_C; j++)
			{
				u_M[i][j] += _M.u_M[i][j];
			}
		}
	}
};