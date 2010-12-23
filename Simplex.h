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


class Simplex {

	public:
		Simplex();

		void init();
		void phase2();

	public:
		std::vector< std::vector< int > > A;
		std::vector< int > b;
		std::vector< int > c;
		std::vector< std::vector< int > > B;
};

#endif
