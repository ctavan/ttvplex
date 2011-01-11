#include "Matrix.h"

using namespace std;

// Constructor
Matrix::Matrix()
{
}

my_rational Matrix::det(vector< vector< my_rational > > A)
{
	return (my_rational)0;
}

void Matrix::transpose(vector< vector< my_rational > > A, vector< vector< my_rational > > At)
{
}

void Matrix::multiply(vector< vector< my_rational > > A, vector< vector< my_rational > > B, vector< vector< my_rational > > R)
{
}

void Matrix::inverse(vector< vector< my_rational > > A, vector< vector< my_rational > > Ai)
{
}

void Matrix::pivot(vector< vector< my_rational > >& B, const vector< vector< my_rational > >& A, const int& i, const int& j)
{
	ldbg << "Pivoting on element: [" << i << "][" << j << "]\n";

	// Make sure the target matrix B holds a copy of the inital matrix A
	B = A;

	for (unsigned r = 0; r < A.size(); r++) {
		for (unsigned s = 0; s < A[r].size(); s++) {
			// We're in the row of the pivot-element. Here only devide by the value of the pivot element
			if ((int)r == i) {
				B[r][s] = A[r][s]/A[i][j];
				continue;
			}
			// Otherwise bring the j'th column of this row to 0
			B[r][s] = A[r][s] - A[i][s] * A[r][j]/A[i][j] ;
		}
	}
}

void Matrix::row_subtract(vector< vector< my_rational > >& A, const int& i, const int& j)
{
	ldbg << "Subtracting: row[" << i << "] - row[" << j << "]\n";

	for (unsigned s = 0; s < A[i].size(); s++) {
		A[i][s] -= A[j][s];
	}
}

// B = A|x
void Matrix::append_vec(vector< vector< my_rational > >& B, const vector< vector< my_rational > >& A, const vector< my_rational >& x)
{
	// Copy contents of matrix A into matrix B
	B = A;
	// Append column x_s to Matrix B
	for (unsigned i = 0; i < x.size(); i++)
	{
		B[i].push_back(x[i]);
	}
}