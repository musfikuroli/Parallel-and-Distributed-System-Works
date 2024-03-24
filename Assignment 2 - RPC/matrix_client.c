#include "matrix.h"

CLIENT *rpc_setup(char *host)
{
	CLIENT *clnt = clnt_create(host, MATRIX_OPERATIONS_PROG, MATRIX_OPERATIONS_VERS, "udp");
	if (clnt == NULL)
	{
		clnt_pcreateerror(host);
		return 0;
	}
	return clnt;
}

// Matrix Addition Handling Function
void matrix_add(CLIENT *clnt, int row1, int col1, int a[], int row2, int col2, int b[])
{
	matrix *result; // Resultant Matrix
	matrices m;		// Struct of 2 Matrices

	// Assigning Values of Matrix 1
	m.row1 = row1;
	m.col1 = col1;
	for (int i = 0; i < row1 * col1; i++)
	{
		m.a[i] = a[i];
	}

	// Assigning Values of Matrix 2
	m.row2 = row2;
	m.col2 = col2;
	for (int i = 0; i < row2 * col2; i++)
	{
		m.b[i] = b[i];
	}

	// Sending Data to Server and Receiving Result
	result = matrix_add_1(&m, clnt);
	if (result == 0)
	{
		clnt_perror(clnt, "call failed");
	}
	else
	{
		// Printing Resultant Matrix
		int count = 0;
		for (int i = 0; i < row1; i++)
		{
			for (int j = 0; j < col1; j++)
			{
				printf("%d\t", result->a[count]);
				count++;
			}
			printf("\n");
		}
	}
}

// Matrix Multiplication Handling Function
void matrix_multiply(CLIENT *clnt, int row1, int col1, int a[], int row2, int col2, int b[])
{
	matrix *result; // Resultant Matrix
	matrices m;		// Struct of 2 Matrices

	// Assigning Values of Matrix 1
	m.row1 = row1;
	m.col1 = col1;
	for (int i = 0; i < row1 * col1; i++)
	{
		m.a[i] = a[i];
	}

	// Assigning Values of Matrix 2
	m.row2 = row2;
	m.col2 = col2;
	for (int i = 0; i < row2 * col2; i++)
	{
		m.b[i] = b[i];
	}

	// Sending Data to Server and Receiving Result
	result = matrix_multiply_1(&m, clnt);
	if (result == 0)
	{
		clnt_perror(clnt, "call failed");
	}
	else
	{
		// Printing Resultant Matrix
		int index = 0;
		for (int i = 0; i < result->row; i++)
		{
			for (int j = 0; j < result->col; j++)
			{
				printf("%d\t", result->a[index]);
				index++;
			}
			printf("\n");
		}
	}
}

// Matrix Inverse Handling Function
void matrix_inverse(CLIENT *clnt, int order, int a[])
{
	matrixInverse *result; // Resultant Matrix
	matrix m;			   // Matrix for Calculating Inverse

	// Assigning Values of Matrix
	m.row = order;
	m.col = order;
	for (int i = 0; i < order * order; i++)
	{
		m.a[i] = a[i];
	}

	// Sending Data to Server and Receiving Result
	result = matrix_inverse_1(&m, clnt); // Use appropriate RPC function for matrix inverse
	if (result->flag == -1)
	{
		// clnt_perror(clnt, "call failed");
		printf("Matrix is not square. Inverse is not possible.\n");
		result->flag = 0;
		exit(0);
	}
	else if (result->flag == -2)
	{
		// clnt_perror(clnt, "call failed");
		printf("Matrix is singular. Inverse is not possible. Client\n");
		result->flag = 0;
		exit(0);
	}
	else
	{
		// Printing Resultant Matrix
		int index = 0;
		for (int i = 0; i < m.row; i++)
		{
			for (int j = 0; j < m.col; j++)
			{
				float value = result->a[index];
				if (value == (int)value)
				{
					printf("%d\t", (int)value);
				}
				else
				{
					printf("%.2f\t", value);
				}
				index++;
			}
			printf("\n");
		}
	}
}

// Matrix Transpose Handling Function
void matrix_transpose(CLIENT *clnt, int row, int col, int a[])
{
	matrix *result; // Resultant Matrix
	matrix m;		// Matrix for Calculating Transpose

	// Assigning Values of Matrix
	m.row = row;
	m.col = col;
	for (int i = 0; i < row * col; i++)
	{
		m.a[i] = a[i];
	}

	// Sending Data to Server and Receiving Result
	result = matrix_transpose_1(&m, clnt); // Use appropriate RPC function for matrix transpose
	if (result == NULL)
	{
		clnt_perror(clnt, "call failed");
	}
	else
	{
		// Printing Resultant Matrix
		int index = 0;
		for (int i = 0; i < m.col; i++)
		{
			for (int j = 0; j < m.row; j++)
			{
				printf("%d\t", result->a[index]);
				index++;
			}
			printf("\n");
		}
	}
}

int main(int argc, char *argv[])
{
	CLIENT *clnt; /* client handle to server */
	char *host;	  /* host */

	int row1, col1, order, a[100];
	int row2, col2, b[100];

	host = argv[1];

	if ((clnt = rpc_setup(host)) == 0)
		exit(1); /* cannot connect */

	int operation;
	printf("Enter operation type:\n");
	printf("1. Matrix Addition\n");
	printf("2. Matrix Multiplication\n");
	printf("3. Matrix Inverse\n");
	printf("4. Matrix Transpose\n");
	scanf("%d", &operation);

	if (operation < 1 || operation > 4)
	{
		printf("Invalid operation type.\n");
		clnt_destroy(clnt);
		exit(1);
	}

	// Operation: Addition
	if (operation == 1)
	{
		printf("Enter The No. of Rows and Cols of Matrices:\n");
		scanf("%d %d", &row1, &col1);

		printf("Enter The Values of Matrix-1:\n");
		for (int i = 0; i < row1 * col1; i++)
		{
			scanf("%d", &a[i]);
		}

		// printf("Enter The No. of Rows and Cols of Matrix-2:\n");
		// scanf("%d %d", &row2, &col2);

		row2 = row1;
		col2 = col1;

		printf("Enter The Values of Matrix-2:\n");
		for (int i = 0; i < row2 * col2; i++)
		{
			scanf("%d", &b[i]);
		}

		matrix_add(clnt, row1, col1, a, row2, col2, b);
	}

	// Operation: Multiplication
	else if (operation == 2)
	{
		printf("Enter The No. of Rows and Cols of Matrix-1:\n");
		scanf("%d %d", &row1, &col1);

		printf("Enter The Values of Matrix-1:\n");
		for (int i = 0; i < row1 * col1; i++)
		{
			scanf("%d", &a[i]);
		}

		printf("Enter The No. of Rows and Cols of Matrix-2:\n");
		scanf("%d %d", &row2, &col2);

		printf("Enter The Values of Matrix-2:\n");
		for (int i = 0; i < row2 * col2; i++)
		{
			scanf("%d", &b[i]);
		}

		matrix_multiply(clnt, row1, col1, a, row2, col2, b);
	}

	// Operation: Inverse
	else if (operation == 3)
	{
		printf("Enter The Order of The Matrix:\n");
		scanf("%d", &order);

		printf("Enter The Values of The Matrix:\n");
		for (int i = 0; i < order * order; i++)
		{
			scanf("%d", &a[i]);
		}
		printf("\n");

		matrix_inverse(clnt, order, a);
	}

	// Operation: Transpose
	else if (operation == 4)
	{
		printf("Enter The No. of Rows and Cols of The Matrix:\n");
		scanf("%d %d", &row1, &col1);

		printf("Enter The Values of The Matrix:\n");
		for (int i = 0; i < row1 * col1; i++)
		{
			scanf("%d", &a[i]);
		}

		matrix_transpose(clnt, row1, col1, a);
	}

	clnt_destroy(clnt);

	exit(0);
}
