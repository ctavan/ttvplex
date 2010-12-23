#include "Log.h"

using namespace std;
using namespace cln;

// Constructor
Log::Log() {
}

void Log::vector(std::vector<int> v, std::string name)
{
	std::cout << "Dumping " << v.size() << "-vector " << name << std::endl;
	for (unsigned i = 0; i < v.size(); i++) {
		std::cout << name << "[" << i << "]\t=>\t" << v[i] << std::endl;
	}
}

void Log::matrix(std::vector< std::vector< int > > m, std::string name)
{
	std::cout << "Dumping " << m.size() << "x" << m[0].size() << " matrix " << name << std::endl;
	for (unsigned i = 0; i < m.size(); i++) {
		for (unsigned j = 0; j < m[i].size(); j++) {
			std::cout << m[i][j] << "\t";
		}
		std::cout << std::endl;
	}
}
