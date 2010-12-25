#ifndef Log_h
#define Log_h

// C Standard libraries
#include <iostream>
#include <vector>
#include <string>


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

using namespace std;
using namespace cln;

class Log {

	public:
		Log();

		void vec(vector< cl_RA > v, string name);
		void matrix(vector< vector<cl_RA> > m, string name);
};

#endif
