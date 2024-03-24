
#include "matrix.h"
#include <stdbool.h>

matrix *
matrix_add_1_svc(matrices *m, struct svc_req *rqstp)
{
	static matrix result;

	/*
	 * insert server code here
	 */

	for (int i = 0; i < m->row1 * m->col1; i++)
	{
		result.a[i] = m->a[i] + m->b[i];
	}

	return &result;
}

matrix *
matrix_multiply_1_svc(matrices *m, struct svc_req *rqstp)
{

	static matrix result;

	if (m->col1 != m->row2)
	{
		fprintf(stderr, "Matrix dimensions are not compatible for multiplication.\n");
		return NULL;
	}

	for (int i = 0; i < m->row1; i++)
	{
		for (int j = 0; j < m->col2; j++)
		{
			int sum = 0;
			for (int k = 0; k < m->col1; k++)
			{
				sum += m->a[i * m->col1 + k] * m->b[k * m->col2 + j];
			}
			result.a[i * m->col2 + j] = sum;
		}
	}

	result.row = m->row1;
	result.col = m->col2;

	return &result;
}

matrix *
matrix_transpose_1_svc(matrix *m, struct svc_req *rqstp)
{
	static matrix result;

	for (int i = 0; i < m->row; i++)
	{
		for (int j = 0; j < m->col; j++)
		{
			result.a[j * m->row + i] = m->a[i * m->col + j];
		}
	}

	result.row = m->col;
	result.col = m->row;

	return &result;
}

//-------------------------------------------------------------------------------------------------------------------
//------------------------------------Inverse Calculation------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------

// Helper function to calculate the determinant of a 2x2 matrix
double determinant_2x2(int a, int b, int c, int d)
{
	return (a * d) - (b * c);
}

// Helper function to calculate the cofactor of a matrix
void cofactor(int mat[100][100], int temp[100][100], int p, int q, int n)
{
	int i = 0, j = 0;

	for (int row = 0; row < n; row++)
	{
		for (int col = 0; col < n; col++)
		{
			if (row != p && col != q)
			{
				temp[i][j++] = mat[row][col];

				if (j == n - 1)
				{
					j = 0;
					i++;
				}
			}
		}
	}
}

// Helper function to calculate the determinant of a matrix
int determinant(int mat[100][100], int n)
{
	if (n == 1)
	{
		return mat[0][0];
	}
	else if (n == 2)
	{
		return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
	}
	else
	{
		int det = 0;
		int temp[100][100];
		int sign = 1;

		for (int f = 0; f < n; f++)
		{
			cofactor(mat, temp, 0, f, n);
			det += sign * mat[0][f] * determinant(temp, n - 1);
			sign = -sign;
		}
		return det;
	}
}

// Helper function to calculate the adjoint of a matrix
void adjoint(int mat[100][100], int adj[100][100], int n)
{
	if (n == 1)
	{
		adj[0][0] = 1;
		return;
	}

	int sign = 1, temp[100][100];

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cofactor(mat, temp, i, j, n);
			sign = ((i + j) % 2 == 0) ? 1 : -1;
			// adj[j][i] = (sign) * (determinant_2x2(temp[0][0], temp[0][1], temp[1][0], temp[1][1]));
			adj[j][i] = (sign) * (determinant(temp, 2));
		}
	}
}

// Helper function to calculate the inverse of a matrix
bool inverse(int mat[100][100], float inverse_mat[100][100], int n)
{
	int det = determinant(mat, n); // Use the determinant function you have defined
	if (det == 0)
	{
		return false;
	}

	int adj[100][100];
	adjoint(mat, adj, n);

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			inverse_mat[i][j] = (float)adj[i][j] / (float)det;
		}
	}
	return true;
}

matrixInverse *
matrix_inverse_1_svc(matrix *m, struct svc_req *rqstp)
{
	static matrixInverse result;

	if (m->row != m->col)
	{
		fprintf(stderr, "Matrix is not square. Inverse is not possible.\n");
		result.flag = -1;
		return &result;
	}

	else if (m->row == 2) // Special case for 2x2 matrix
	{
		int det = determinant_2x2(m->a[0], m->a[1], m->a[2], m->a[3]);
		if (det == 0)
		{
			fprintf(stderr, "Matrix is singular. Inverse is not possible. Server 2x2\n");
			result.flag = -2;
			return &result;
		}
		else
		{
			float inverse_mat[4];
			inverse_mat[0] = m->a[3] / (float)det;
			inverse_mat[1] = -m->a[1] / (float)det;
			inverse_mat[2] = -m->a[2] / (float)det;
			inverse_mat[3] = m->a[0] / (float)det;

			for (int i = 0; i < m->row; i++)
			{
				for (int j = 0; j < m->col; j++)
				{
					result.a[i * m->col + j] = inverse_mat[i * m->col + j];
				}
			}

			result.row = m->row;
			result.col = m->col;

			return &result;
		}
	}

	else
	{
		// Rest of the code for general matrix inversion
		int mat[100][100];
		for (int i = 0; i < m->row; i++)
		{
			for (int j = 0; j < m->col; j++)
			{
				mat[i][j] = m->a[i * m->col + j];
			}
		}

		float inverse_mat[100][100];
		if (!inverse(mat, inverse_mat, m->row))
		{
			fprintf(stderr, "Matrix is singular. Inverse is not possible. Server 3x3\n");
			result.flag = -2;
			return &result;
		}

		for (int i = 0; i < m->row; i++)
		{
			for (int j = 0; j < m->col; j++)
			{
				result.a[i * m->col + j] = inverse_mat[i][j];
			}
		}

		result.row = m->row;
		result.col = m->col;

		return &result;
	}
}
