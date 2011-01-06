#ifndef LPParser_h
#define LPParser_h

#include "global.h"

#include <algorithm>

using namespace std;
using namespace cln;

struct LPVariable
{
	string name;
	mpq_class coeff;

	LPVariable() : name(""), coeff(1) {}

	void dump()
	{
		linf << "\tCoeff:\t" << coeff << "\n";
		linf << "\tName:\t" << name << "\n";
	}
};

struct LPObjective
{
	int direction;
	string name;
	vector<LPVariable> elements;

	LPObjective() : direction(0), name("obj") {}

	void dump()
	{
		linf << "Direction:\t" << (direction == 1 ? "MAX" : "MIN") << "\n";
		linf << "Name:\t\t" << name << "\n";
		linf << "Elements:\n";
		for (unsigned i = 0; i < elements.size(); i++)
		{
			elements[i].dump();
		}
	}
};

struct LPConstraint
{
	int relation;					//!< Relation: -1 for <, 0 for =, 1 for >
	string name;					//!< Name of the constraint
	mpq_class rhs;					//!< Right hand side
	vector<LPVariable> elements;	//!< Variables that are being used in the constraint

	LPConstraint() : relation(0), name(""), rhs(0) {}

	void dump()
	{
		linf << "Name:\t\t" << name << "\n";
		linf << "Elements:\n";
		for (unsigned i = 0; i < elements.size(); i++)
		{
			elements[i].dump();
		}
		linf << "Relation:\t" << (relation == -1 ? "<" : (relation == 1 ? ">" : "=")) << "\n";
		linf << "RHS:\t\t" << rhs << "\n";
	}
};

struct LPBound
{
	string name;				//!< Name of the variable
	mpq_class lower;			//!< Lower bound
	mpq_class upper;			//!< Upper bound
	bool lower_unbound;			//!< Whether the variable is unbounded from below
	bool upper_unbound;			//!< Whether the variable is unbounded from above

	LPBound() : name(""), lower(0), upper(0), lower_unbound(false), upper_unbound(true) {}

	void dump()
	{
		linf << "Name:\t\t" << name << "\n";
		linf << "Lower:\t\t" << lower << " (" << (lower_unbound ? "unbounded" : "") << ")\n";
		linf << "Upper:\t\t" << upper << " (" << (upper_unbound ? "unbounded" : "") << ")\n";
	}
};

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
class LPParser
{

	private:
		ifstream lpfile;

		static const int SEC_START = 1;
		static const int SEC_OBJECTIVE = 2;
		static const int SEC_CONSTRAINTS = 3;
		static const int SEC_BOUNDS = 4;
		static const int SEC_GENERALS = 5;
		static const int SEC_BINARIES = 6;
		static const int SEC_END = 7;
		int section;

		static const int OBJ_MIN = 0;
		static const int OBJ_MAX = 1;

		static const int REL_LE = -1;
		static const int REL_EQ = 0;
		static const int REL_GE = 1;

		string trim(string line);
		void trim(vector<string>& lines);
		vector<string> split_expression(const string& str, const string& delimiters = "+-");
		void parse_varcoeff(string str, vector<LPVariable>& elements);

	public:
		LPObjective objective;
		vector<LPConstraint> constraints;
		vector<LPBound> bounds;

		LPParser(string filename);
		~LPParser();
		void read();
};

#endif
