#include "Log.h"

using namespace std;
using namespace cln;

// Constructor
Log::Log() {
}

void Log::message(string text)
{
	cout << text << endl;
}

void Log::vec(vector< cl_RA > v, string name)
{
	cout << "Dumping " << v.size() << "-vector " << name << ":" << endl;
	for (unsigned i = 0; i < v.size(); i++) {
		// cout << name << "[" << i << "]\t=>\t" << ;
		cout << v[i] << endl;
	}
}
void Log::vec(vector< unsigned > v, string name)
{
	cout << "Dumping " << v.size() << "-vector " << name << ":" << endl;
	for (unsigned i = 0; i < v.size(); i++) {
		cout << v[i] << endl;
	}
}

void Log::matrix(vector< vector< cl_RA > > m, string name)
{
	cout << "Dumping " << m.size() << "x" << m[0].size() << " matrix " << name << ":" << endl;
	for (unsigned i = 0; i < m.size(); i++) {
		for (unsigned j = 0; j < m[i].size(); j++) {
			cout << m[i][j] << "\t";
		}
		cout << endl;
	}
}
