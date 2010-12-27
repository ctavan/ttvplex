#ifndef Log_h
#define Log_h


#include <stdio.h>

// Some preprocessor macros for debugmessages
#define DEBUG_LEVEL 3
#define DEBUG_LINES 1

#define DEBUGPRINT2(...)       fprintf(stderr, __VA_ARGS__)
// #if DEBUG_LINES >= 1
#define WHERESTR  "[file %s, line %d]: "
#define WHEREARG  __FILE__, __LINE__
#define DEBUGPRINT(_fmt, ...)  DEBUGPRINT2(WHERESTR _fmt, WHEREARG, __VA_ARGS__)
// #else
// #define DEBUGPRINT(_fmt, ...)  DEBUGPRINT2(_fmt, __VA_ARGS__)
// #endif

#if DEBUG_LEVEL >= 1
#define WARN(...) DEBUGPRINT(__VA_ARGS__)
#else
#define WARN(...)
#endif

#if DEBUG_LEVEL >= 2
#define INFO(...) DEBUGPRINT(__VA_ARGS__)
#else
#define INFO(...)
#endif

#if DEBUG_LEVEL >= 3
#define DEBUG(...) DEBUGPRINT(__VA_ARGS__)
#else
#define DEBUG(...)
#endif


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

/** \brief  Logging class
	
	Allows dumping/logging matrices and vectors in a convenient way.
	
© Copyright 2010 TU Berlin - Christoph Tavan. All Rights Reserved.

	\author Christoph Tavan TU Berlin
	\author $LastChangedBy$
	\date 2010-12-27
	\date $LastChangedDate$
	\version $Rev$	\sa
**/
class Log {

	public:
		Log();

/** \brief Dump some string.
	
	\author Christoph Tavan TU Berlin
	\date 2010-12-27
	\param string Message
	\param  description of parameter
	\return description of return value
	\sa
**/
		static void message(string text);
/** \brief Dump a 1D-vector
	
	\author Christoph Tavan TU Berlin
	\date 2010-12-27
	\param  vector Vector
	\param  string Name of the vector
	\return void
	\sa
**/
		static void vec(vector< cl_RA > v, string name);
/** \brief Dump a 2D-matrix
	
	\author Christoph Tavan TU Berlin
	\date 2010-12-27
	\param  vector m x n matrix
	\param  string Name of the matrix
	\return description of return value
	\sa
**/
		static void matrix(vector< vector<cl_RA> > m, string name);
};

#endif
