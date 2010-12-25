#ifndef Simplex_h
#define Simplex_h

// C Standard libraries
#include <iostream>
#include <vector>


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

/** \brief  Revised Simplex Algorithm
	
	Solves linear programs in standard form using the revised simplex algorithm.
	
© Copyright 2010 TU Berlin - Christoph Tavan. All Rights Reserved.

	\author Christoph Tavan TU Berlin
	\author $LastChangedBy$
	\date 2010-12-25
	\date $LastChangedDate$
	\version $Rev$	\sa
**/
class Simplex {

	public:
		Simplex();

/** \brief Initialize the tableaus
	
		Initialize the tableaus for the revised simplex algorithm.
	
	\author Christoph Tavan TU Berlin
	\date 2010-12-25
	\return void
	\sa
**/
		void init();
		void phase2();

		vector< vector< cl_RA > > A; //!< Initial coefficient matrix
		vector< cl_RA > b; //!< Initial righthand side
		vector< cl_RA > c; //!< Initial cost coefficients
		vector< vector< cl_RA > > CARRY; //!< Carry^0-Matrix
		vector< int > basis; //!< Index-set of current matrix columns
};

#endif
