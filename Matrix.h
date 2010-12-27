#ifndef Matrix_h
#define Matrix_h

// C Standard libraries
#include <iostream>
#include <vector>
#include <string>
#include <sstream>


// We work with integers and real numbers.
#include <cln/integer.h>
#include <cln/rational.h>
#include <cln/real.h>
// We do I/O.
#include <cln/io.h>
#include <cln/integer_io.h>
#include <cln/rational_io.h>
#include <cln/real_io.h>
// We use the timing functions.
#include <cln/timing.h>


// Local includes
#include "Log.h"
#include "Matrix.h"


using namespace std;
using namespace cln;

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
	\return cl_RA Value of the determinant
	\sa
**/
		cl_RA det(vector< vector< cl_RA > > A);
/** \brief Transpose matrix
	
	Transpose matrix A and write the transposed matrix to At.
	
	\author Christoph Tavan TU Berlin
	\date 2010-12-25
	\param  vector m x m Matrix A
	\param  vector Resulting m x m Matrix At
	\return void
	\sa
**/
		void transpose(vector< vector< cl_RA > > A, vector< vector< cl_RA > > At);
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
		void multiply(vector< vector< cl_RA > > A, vector< vector< cl_RA > > B, vector< vector< cl_RA > > R);
/** \brief Invert matrix
	
	Compute the inverse of matrix A and write it into Ai.
	
	\author Christoph Tavan TU Berlin
	\date 2010-12-25
	\param  vector m x m Matrix A
	\param  vector m x m Matrix Ai
	\return void
	\sa
**/
		void inverse(vector< vector< cl_RA > > A, vector< vector< cl_RA > > Ai);
/** \brief Perform pivot operation
	
	Perform a pivot operation on the given pivot-element ij (indices starting at 0)
	
	\author Christoph Tavan TU Berlin
	\date 2010-12-25
	\param  vector Initial m x n Matrix A
	\param  vector Final m x n Matrix B after pivoting
	\param  int i Row-index of pivot element
	\param  int j Column-index of pivot element
	\return void
	\sa
**/
		static void pivot(vector< vector< cl_RA > > A, vector< vector< cl_RA > > & B, int i, int j);
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
		static void rowSubtract(vector< vector< cl_RA > > & A, int i, int j);
};

#endif
