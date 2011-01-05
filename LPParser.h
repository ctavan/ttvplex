#ifndef LPParser_h
#define LPParser_h

#include "global.h"

#include <algorithm>

using namespace std;
using namespace cln;

/** \brief  Class for parsing LP-Files
	
	Reads LP-files, converts the LP to standard form and returns the arrays that are
	needed in the simplex algorithm.
	
© Copyright 2011 TU Berlin - Christoph Tavan. All Rights Reserved.

	\author Christoph Tavan TU Berlin
	\author $LastChangedBy$
	\date 2011-01-04
	\date $LastChangedDate$
	\version $Rev$	\sa
**/
class LPParser {

	private:
		ifstream lpfile;

		static const int SEC_START = 1;
		static const int SEC_OBJECTIVE = 2;
		static const int SEC_CONSTRAINTS = 3;
		static const int SEC_BOUNDS = 4;
		static const int SEC_END = 5;
		int section;

		string clean_line(string line);

	public:
		LPParser(string filename);
		~LPParser();
		void read();
};

#endif
