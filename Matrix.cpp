#include "Matrix.h"

using namespace std;

// Constructor
Matrix::Matrix()
{
}

mpq_class Matrix::det(vector< vector< mpq_class > > A)
{
	return (mpq_class)0;
}

void Matrix::transpose(vector< vector< mpq_class > > A, vector< vector< mpq_class > > At)
{
}

void Matrix::multiply(vector< vector< mpq_class > > A, vector< vector< mpq_class > > B, vector< vector< mpq_class > > R)
{
}

void Matrix::inverse(vector< vector< mpq_class > > A, vector< vector< mpq_class > > Ai)
{
}

void Matrix::pivot(vector< vector< mpq_class > >& B, const vector< vector< mpq_class > >& A, const int& i, const int& j)
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

void Matrix::row_subtract(vector< vector< mpq_class > >& A, const int& i, const int& j)
{
	ldbg << "Subtracting: row[" << i << "] - row[" << j << "]\n";

	for (unsigned s = 0; s < A[i].size(); s++) {
		A[i][s] -= A[j][s];
	}
}

// B = A|x
void Matrix::append_vec(vector< vector< mpq_class > >& B, const vector< vector< mpq_class > >& A, const vector< mpq_class >& x)
{
	// Copy contents of matrix A into matrix B
	B = A;
	// Append column x_s to Matrix B
	for (unsigned i = 0; i < x.size(); i++)
	{
		B[i].push_back(x[i]);
	}
}