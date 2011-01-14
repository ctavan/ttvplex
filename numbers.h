#ifndef numbers_h
#define numbers_h

// GMP: The GNU Multiple Precision Arithmetic Library
#include <gmpxx.h>

typedef mpq_class my_rational;	//!< Redefine variable type for exact rartional numbers
typedef mpf_class my_float;		//!< Redefine variable type for arbitrary precision floats

#endif