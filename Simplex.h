#ifndef Simplex_h
#define Simplex_h

#include <iostream>
#include <vector>

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
