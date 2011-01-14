#include "Log.h"

using namespace std;

// Constructor
Log::Log(ostream& out, int level): ostream(out.rdbuf()), level(level) {
	return;
}
Log::~Log()
{
}

void Log::message(string text)
{
	if (level < 1) {
		return;
	}
	(ostream&)*this << text << endl;
}

void Log::vec(vector< my_rational > v, string name)
{
	if (level < 1) {
		return;
	}
	(ostream&)*this << "Dumping " << v.size() << "-vector " << name << ":" << endl;
	for (unsigned i = 0; i < v.size(); i++) {
		(ostream&)*this << v[i] << endl;
	}
}
void Log::vec(vector< unsigned > v, string name)
{
	if (level < 1) {
		return;
	}
	(ostream&)*this << "Dumping " << v.size() << "-vector " << name << ":" << endl;
	for (unsigned i = 0; i < v.size(); i++) {
		(ostream&)*this << v[i] << endl;
	}
}
void Log::vec(vector< string > v, string name)
{
	if (level < 1) {
		return;
	}
	(ostream&)*this << "Dumping " << v.size() << "-vector " << name << ":" << endl;
	for (unsigned i = 0; i < v.size(); i++) {
		(ostream&)*this << v[i] << endl;
	}
}
void Log::matrix(vector< vector< my_rational > > m, string name, bool tofloat)
{
	if (level < 1) {
		return;
	}
	(ostream&)*this << "Dumping " << m.size() << "x" << m[0].size() << " matrix " << name << ":" << endl;
	for (unsigned i = 0; i < m.size(); i++) {
		for (unsigned j = 0; j < m[i].size(); j++) {
			if (tofloat)
			{
				(ostream&)*this << (my_float)m[i][j] << "\t";
			}
			else
			{
				(ostream&)*this << (my_rational)m[i][j] << "\t";
			}
		}
		(ostream&)*this << endl;
	}
}