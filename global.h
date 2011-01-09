#ifndef Global_h
#define Global_h

// C Standard libraries
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

// GMP: The GNU Multiple Precision Arithmetic Library
#include <gmpxx.h>

// Local includes
#include "Log.h"
#include "Matrix.h"


// Global variables for logging/output
extern Log lout;	//!< Standard output of the program (not controlled by verbosity), shown if verbosity >= 1 (default)
extern Log lerr;	//!< Error output (not controlled by verbosity), shown if verbosity >= 1 (default)
extern Log linf;	//!< Info output, shown if verbosity >= 2
extern Log ldbg;	//!< Debug output, shown if verbosity >= 3

#endif