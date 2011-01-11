#ifndef Matrix_h
#define Matrix_h

#include "global.h"

using namespace std;

/** \brief  Matrix manipulation class
	
	Class for matrix manipulation (like inverting, calculating determinants, etc.)
	
© Copyright 2010 TU Berlin - Christoph Tavan. All Rights Reserved.

	\author Christoph Tavan TU Berlin
	\author $LastChangedBy$
	\date 2010-12-27
	\date $LastChangedDate$
	\version $Rev$	\sa
**/
class Matrix {

	public:
		Matrix();

/** \brief Determinant of a matrix
	
	Computes and returns the determinant of an m x m Matrix A.
	
	\author Christoph Tavan TU Berlin
	\date 2010-12-25
	\param  vector m x m Matrix A
	\return mpq_class Value of the determinant
	\sa
**/
		mpq_class det(vector< vector< mpq_class > > A);
/** \brief Transpose matrix
	
	Transpose matrix A and write the transposed matrix to At.
	
	\author Christoph Tavan TU Berlin
	\date 2010-12-25
	\param  vector m x m Matrix A
	\param  vector Resulting m x m Matrix At
	\return void
	\sa
**/
		void transpose(vector< vector< mpq_class > > A, vector< vector< mpq_class > > At);
/** \brief Matrix multiplication
	
	Multiply matrix A with matrix B and write result to matrix R.
	
	\author Christoph Tavan TU Berlin
	\date 2010-12-25
	\param  vector m x m Matrix A
	\param  vector m x m Matrix B
	\param  Resulting m x m Matrix R = A . B
	\return void
	\sa
**/
		void multiply(vector< vector< mpq_class > > A, vector< vector< mpq_class > > B, vector< vector< mpq_class > > R);
/** \brief Invert matrix
	
	Compute the inverse of matrix A and write it into Ai.
	
	\author Christoph Tavan TU Berlin
	\date 2010-12-25
	\param  vector m x m Matrix A
	\param  vector m x m Matrix Ai
	\return void
	\sa
**/
		void inverse(vector< vector< mpq_class > > A, vector< vector< mpq_class > > Ai);
/** \brief Perform pivot operation
	
	Perform a pivot operation on the given pivot-element ij (indices starting at 0)
	
	\author Christoph Tavan TU Berlin
	\date 2010-12-25
	\param  vector Final m x n Matrix B after pivoting
	\param  vector Initial m x n Matrix A
	\param  int i Row-index of pivot element
	\param  int j Column-index of pivot element
	\return void
	\sa
**/
		static void pivot(vector< vector< mpq_class > >& B, const vector< vector< mpq_class > >& A, const int& i, const int& j);
/** \brief Subtract row j from row i
	
	Subtracts row j of input matrix A from row i. The input matrix is manipulated directly.
	
	\author Christoph Tavan TU Berlin
	\date 2010-12-27
	\param vector m x n Matrix A
	\param int i Row-index of first row
	\param int j Row-index of second row
	\return void
	\sa
**/
		static void row_subtract(vector< vector< mpq_class > >& A, const int& i, const int& j);
/** \brief Append vector x to matrix A
	
	Appends vector x to matrix A and writes the result to matrix B: B = A|x
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-11
	\param vector Resulting matrix B
	\param vector Initial matrix A
	\param vector Column-vector to be appended to A
	\return void
	\sa
**/
		static void append_vec(vector< vector< mpq_class > >& B, const vector< vector< mpq_class > >& A, const vector< mpq_class >& x);
};

#endif
