#ifndef Log_h
#define Log_h

#include <iostream>
#include <vector>
#include <string>

class Log {

	public:
		Log();

		void vector(std::vector<int> v, std::string name);
		void matrix(std::vector< std::vector< int > > m, std::string name);
};

#endif
