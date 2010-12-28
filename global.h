#ifndef Global_h
#define Global_h

// C Standard libraries
#include <iostream>
#include <vector>
#include <string>

// Class library for numbers
#include "cln.h"

// Local includes
#include "Log.h"
#include "Matrix.h"
#include "Simplex.h"


// Global variables (just for logging/output)
extern Log lout;
extern Log ldbg;
extern Log lerr;

#endif