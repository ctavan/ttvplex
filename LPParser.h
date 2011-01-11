#ifndef LPParser_h
#define LPParser_h

#include "global.h"

#include <algorithm>

using namespace std;

/** \brief  Class holding a coefficient-variable-pair
	
	Holds a pair consisting of a variable defined by `name` with a coefficient
	as they occur in the objective and in the constraints. E.g. out of
	  1 x1 - 5 x3 >= 30
	two variable pairs, say LPvariable var1 and LPVariable var2 can be extracted:
	  var1.name = x1
	  var1.coeff = 1
	  var2.name = x3
	  var2.coeff = -5
	
© Copyright 2011 TU Berlin - Christoph Tavan. All Rights Reserved.

	\author Christoph Tavan TU Berlin
	\author $LastChangedBy$
	\date 2011-01-06
	\date $LastChangedDate$
	\version $Rev$	\sa
**/
struct LPVariable
{
	string name;		//!< Variable name
	my_rational coeff;	//!< Coefficient in front of the variable

	LPVariable() : name(""), coeff(1) {}

/** \brief Convenience method to dump an object of type LPVariable
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-06
	\return void
	\sa
**/
	void dump(Log& out, const bool& detailed = false)
	{
		if (detailed)
		{
			out << "\tCoeff:\t" << coeff << "\n";
			out << "\tName:\t" << name << "\n";
		}
		else
		{
			out << "+ " << coeff << " " << name << " ";
		}
	}
};

/** \brief  Class holding an objective
	
	Holds the objective of an LP. The direction (MINIMIZE or MAXIMIZE) is stored
	along with an optional name.
	The variables with their coefficients are stored in the property elements.
	
© Copyright 2011 TU Berlin - Christoph Tavan. All Rights Reserved.

	\author Christoph Tavan TU Berlin
	\author $LastChangedBy$
	\date 2011-01-06
	\date $LastChangedDate$
	\version $Rev$	\sa
**/
struct LPObjective
{
	int direction;					//!< Whether to maximize or minimize the objective. Default: MINIMIZE
	string name;					//!< Optional name of the Objective. Default: "obj"
	vector<LPVariable> elements;	//!< Coefficient-variable pairs of the objective.
	my_rational offset;				//!< Offset to the objective that may come from substituting variables with nonzero lower bound

	static const int OBJ_MIN = 0;	//!< Constant designating MINIMIZE
	static const int OBJ_MAX = 1;	//!< Constant designating MAXIMIZE
/** \brief Constructor
	
	Member Variables are initialized as follows:
		name = "obj"
		direction = MINIMIZE
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-06
	\return void
	\sa
**/
	LPObjective() : direction(OBJ_MIN), name("obj"), offset(0) {}
/** \brief Convenience method to dump an object of type LPObjective
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-06
	\return void
	\sa
**/
	void dump(Log& out, const bool& detailed = false)
	{
		if (detailed)
		{
			out << "Direction:\t" << (direction == OBJ_MAX ? "MAX" : "MIN") << "\n";
			out << "Name:\t\t" << name << "\n";
			out << "Offset:\t\t" << offset << "\n";
			out << "Elements:\n";
			for (unsigned i = 0; i < elements.size(); i++)
			{
				elements[i].dump(out, detailed);
			}
		}
		else
		{
			out << (direction == OBJ_MAX ? "MAX" : "MIN") << "\n";
			out << name << ": ";
			for (unsigned i = 0; i < elements.size(); i++)
			{
				elements[i].dump(out, detailed);
			}
			out << " + " << offset << "\n";
		}
	}
};

/** \brief  Class holding a constraint
	
	Holds a constraint from the "SUBJECT TO" block.
	 * The lefthand-side (LHS) of a constraint is split into coefficient-variable pairs
	stored in the property elements.
	 * The relation between LHS and RHS (right hand side) is stored in the property
	relation.
	 * The RHS is stored in the property rhs.
	 * If an optional name was defined, it is stored in the property name.
	
	
© Copyright 2011 TU Berlin - Christoph Tavan. All Rights Reserved.

	\author Christoph Tavan TU Berlin
	\author $LastChangedBy$
	\date 2011-01-06
	\date $LastChangedDate$
	\version $Rev$	\sa
**/
struct LPConstraint
{
	int relation;					//!< Relation: -1 for <, 0 for =, 1 for >
	string name;					//!< Name of the constraint
	my_rational rhs;					//!< Right hand side
	vector<LPVariable> elements;	//!< Variables that are being used in the constraint

	static const int REL_LE = -1;	//!< Constant for "less than or equal"-relations
	static const int REL_EQ = 0;	//!< Constant for "equal"-relations
	static const int REL_GE = 1;	//!< Constant for "greater than or equal"-relations
/** \brief Constructor
	
	Member Variables are initialized as follows:
		name = ""
		relation = "="
		rhs = 0
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-06
	\return void
	\sa
**/
	LPConstraint() : relation(REL_EQ), name(""), rhs(0) {}
/** \brief Convenience method to dump an object of type LPConstraint
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-06
	\return void
	\sa
**/
	void dump(Log& out, const bool& detailed = false)
	{
		if (detailed)
		{
			out << "Name:\t\t" << name << "\n";
			out << "Elements:\n";
			for (unsigned i = 0; i < elements.size(); i++)
			{
				elements[i].dump(out, detailed);
			}
			out << "Relation:\t" << (relation == REL_LE ? "<" : (relation == REL_GE ? ">" : "=")) << "\n";
			out << "RHS:\t\t" << rhs << "\n";
		}
		else
		{
			out << name << ": ";
			for (unsigned i = 0; i < elements.size(); i++)
			{
				elements[i].dump(out, detailed);
			}
			out << " " << (relation == REL_LE ? "<" : (relation == REL_GE ? ">" : "=")) << " " << rhs << "\n";
		}
	}
};

/** \brief  Class holding a bound
	
	Holds the bounds for one variable. The variable is identified by the name attribute
	lower and upper bounds are stored as the properties lower and upper if they are finite.
	If lower or upper bound are inifinity, lower_unbound or upper_unbound must be set to
	true respectively. If lower/upper_unbound == true, the values in lower/upper should be
	ignored.
	
© Copyright 2011 TU Berlin - Christoph Tavan. All Rights Reserved.

	\author Christoph Tavan TU Berlin
	\author $LastChangedBy$
	\date 2011-01-06
	\date $LastChangedDate$
	\version $Rev$	\sa
**/
struct LPBound
{
	string name;				//!< Name of the variable
	my_rational lower;			//!< Lower bound
	my_rational upper;			//!< Upper bound
	bool lower_unbound;			//!< Whether the variable is unbounded from below
	bool upper_unbound;			//!< Whether the variable is unbounded from above
/** \brief Constructor
	
	Member Variables are initialized as follows:
		name = ""
		lower = 0
		lower_unbound = false
		upper = 0
		upper_unbound = true
	meaning that all variables are positive integers but not bound from above.
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-06
	\return void
	\sa
**/
	LPBound() : name(""), lower(0), upper(0), lower_unbound(false), upper_unbound(true) {}
/** \brief Convenience method to dump an object of type LPBound
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-06
	\return void
	\sa
**/
	void dump(Log& out, const bool& detailed = false)
	{
		if (detailed)
		{
			out << "Name:\t\t" << name << "\n";
			out << "Lower:\t\t" << lower << " (" << (lower_unbound ? "unbounded" : "") << ")\n";
			out << "Upper:\t\t" << upper << " (" << (upper_unbound ? "unbounded" : "") << ")\n";
		}
		else
		{
			stringstream low;
			low << lower;
			stringstream up;
			up << upper;
			out << (lower_unbound ? "-INF" : low.str()) << " <= " << name << " <= " << (upper_unbound ? "+INF" : up.str()) << "\n";
		}
	}
};

/** \brief  Class holding a list of variables
	
	Holds a list of all variable-names in the problem. Has convenience methods for
	accessing the variablenames.
	
© Copyright 2011 TU Berlin - Christoph Tavan. All Rights Reserved.

	\author Christoph Tavan TU Berlin
	\author $LastChangedBy$
	\date 2011-01-08
	\date $LastChangedDate$
	\version $Rev$	\sa
**/
struct LPVarlist
{
	vector<string> elements;	//!< Vector of variable names
	int ns;						//!< Number of slack and surplus variables
	string s_prefix;			//!< String prefix of slack and surplus variables
	int nbound;					//!< Number of variables that have been introduced for nonzero lower bounds
	string bound_prefix;		//!< Prefix variables that have nonzero lower bound are replaced with
	int nsplit;					//!< Number of unbounded variables that have been split into two vars >= 0
	string split_prefix_p;		//!< Prefix for the plus part of a split unbounded variable
	string split_prefix_m;		//!< Prefix for the minus part of a split unbounded variable
/** \brief Constructor
	
	Member Variables are initialized as follows:
		ns = 0
		s_prefix = s
	meaning that all variables are positive integers but not bound from above.
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-06
	\return void
	\sa
**/
	LPVarlist() : ns(0), s_prefix("s_"), nbound(0), bound_prefix("b_"), nsplit(0), split_prefix_p("plus_"), split_prefix_m("minus_") {}
/** \brief Adds a variable named by name to the list only if it is not yet in the list
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-08
	\param string& Variable to be added to the list
	\return Index of the variable
	\sa
**/
	int add(const string& name)
	{
		int i;
		if ((i = indexOf(name)) < 0)
		{
			elements.push_back(name);
			i = elements.size()-1;
		}
		return i;
	}
/** \brief Returns the index of the variable named by the first argument
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-08
	\param string& Variable name to check for
	\return index of the variable or -1 if the variable doesnt exist
	\sa
**/
	int indexOf(const string& name, const bool& containing = false)
	{
		for (unsigned i = 0; i < elements.size(); i++)
		{
			if (   (!containing && elements[i] == name)
				|| (containing && elements[i].find(name) != string::npos))
			{
				return i;
			}
		}
		return -1;
	}
/** \brief Determines a unique slack/surplus variable name and adds it to the variable list
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-08
	\return Name of the slack/surplus variable
	\sa
**/
	string addSlackSurplus()
	{
		// First determine the slack/surplus-variable prefix (must be unique)
		if (ns < 1)
		{
			ldbg << "Determining unique slack variable prefix.";
			// Append '_' to the s_prefix, until it is assured, that unique slack/surplus-variable-names will be generated
			while (indexOf(s_prefix, true) >= 0)
			{
				ldbg << "Is there a variable containing the slack/surplus-prefix '" << s_prefix << "'?" << indexOf(s_prefix, true) << "\n";
				s_prefix += "_";
			}
			ldbg << "Found unique slack/surplus-prefix '" << s_prefix << "'\n";
		}
		string name;
		stringstream out;
		out << ns++;
		name = s_prefix + out.str();
		elements.push_back(name);
		ldbg << "Adding slack/surplus-variable: '" << name << "', now having " << ns << " slack/surplus variable(s) in the system.\n";
		return name;
	}
	string replaceBounded(const string& name)
	{
		// First determine the slack/surplus-variable prefix (must be unique)
		if (nbound < 1)
		{
			ldbg << "Determining unique bound variable prefix.\n";
			// Append '_' to the s_prefix, until it is assured, that unique slack/surplus-variable-names will be generated
			while (indexOf(bound_prefix, true) >= 0)
			{
				ldbg << "Is there a variable containing the slack/surplus-prefix '" << bound_prefix << "'?" << indexOf(bound_prefix, true) << "\n";
				bound_prefix += "_";
			}
			ldbg << "Found unique bound-prefix '" << bound_prefix << "'\n";
		}
		nbound++;
		int i = indexOf(name);
		elements[i] = bound_prefix + name;
		return elements[i];
	}
	void splitPlusMinus(const string& name)
	{
		// First determine the slack/surplus-variable prefix (must be unique)
		if (nsplit < 1)
		{
			ldbg << "Determining unique bound variable prefix.\n";
			// Append '_' to the split_prefix_p, until it is assured that unique variables are generated
			while (indexOf(split_prefix_p, true) >= 0)
			{
				ldbg << "Is there a variable containing the split-prefix '" << split_prefix_p << "'?" << indexOf(split_prefix_p, true) << "\n";
				split_prefix_p += "_";
			}
			ldbg << "Found unique bound-prefix '" << split_prefix_p << "'\n";
			// Append '_' to the split_prefix_p, until it is assured that unique variables are generated
			while (indexOf(split_prefix_m, true) >= 0)
			{
				ldbg << "Is there a variable containing the split-prefix '" << split_prefix_m << "'?" << indexOf(split_prefix_m, true) << "\n";
				split_prefix_m += "_";
			}
			ldbg << "Found unique bound-prefix '" << split_prefix_m << "'\n";
		}
		nsplit++;
		int i = indexOf(name);
		elements[i] = split_prefix_p + name;
		string minus_name;
		minus_name = split_prefix_m + name;
		elements.push_back(minus_name);
	}
/** \brief Convenience method to dump an object of type LPVarlist
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-06
	\return void
	\sa
**/
	void dump(Log& out)
	{
		for (unsigned i = 0; i < elements.size(); i++)
		{
			out << elements[i] << "\n";
		}
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
		ifstream lpfile;						//!< Input filestream for the LP-file to be read

		static const int SEC_START = 1;			//!< Constant for a block in the LP-file
		static const int SEC_OBJECTIVE = 2;		//!< Constant for a block in the LP-file
		static const int SEC_CONSTRAINTS = 3;	//!< Constant for a block in the LP-file
		static const int SEC_BOUNDS = 4;		//!< Constant for a block in the LP-file
		static const int SEC_GENERALS = 5;		//!< Constant for a block in the LP-file
		static const int SEC_BINARIES = 6;		//!< Constant for a block in the LP-file
		static const int SEC_END = 7;			//!< Constant for a block in the LP-file
		int section;							//!< Section that is currently being processed

/** \brief Cleans up a string
	
	The input string is transformed in the following way and a copy with the
	transformed string is returned:
	 * Strip comments (i.e. everything following "\")
	 * Replace tabs by spaces
	 * Remove leading and trailing spaces
	 * Transform everything to lowercase
	 * Remove all remaining spaces (only if strip_spaces == true)
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-06
	\param string Input string to be trimmed
	\return string Trimmed string
	\sa
**/
		string trim(string line, const bool& strip_spaces = true);
/** \brief Cleans a whole vector of strings
	
	Passes a whole vector of strings by reference to the trim method above and
	modifies all elements.
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-06
	\param  vector<string>& Vector of strings that are all trimmed
	\return void
	\sa
**/
		void trim(vector<string>& lines);
/** \brief Splits an expression into useful chunks.
	
	Takes in expression from the objective-, constraint- or bounds-block of an LP-file
	and splits it into useful chunks.
	Note that:
	 * the name of a line (everything up to the first colon) has to be stripped
	before.
	 * the string must not contain any whitespaces.
	 * 2-char relation-signs must have been translated to 1-char relation signs.
	  * i.e.: replace >= and => by > and replace <= and =< by <
	Example:
	 Input: 3x+7y-8z>10
	 Output: {'3x', '+', '7y', '-', '8z', '>', '10'}
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-06
	\param string& The string containing coefficients, variables and symbols to be split
	\param string& Delimiters to be used for splitting
	\return Vector of strings resulting from splitting the input string
	\sa
**/
		vector<string> split_expression(const string& str, const string& delimiters = "+-");
/** \brief Generates LPVariables from coefficient/variable pairs extracted by split_expression()
	
	Receives the chunks of a vector generated by split_expression and writes
	LPVariable's to the elements-object that has to be passed by reference.
	All signs are taken care of.
	Example:
	 Input-sequence: {'3x', '+', '7y', '-', '8z'}
	 Output: the elements-vector now has three LPVariable-elements:
	  elements[0].coeff = "3"
	  elements[0].name = "x"
	  elements[1].coeff = "7"
	  elements[1].name = "y"
	  elements[2].coeff = "-8"
	  elements[2].name = "z"
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-06
	\param string Variable-coefficient pair
	\param vector<LPVariable>& Vector of variables
	\return void
	\sa
**/
		void parse_varcoeff(string str, vector<LPVariable>& elements);

	public:
		LPObjective objective;				//!< The objective
		vector<LPConstraint> constraints;	//!< The constraints
		vector<LPBound> bounds;				//!< The bounds
		LPVarlist variables;				//!< List of all variable-names in the LP

/** \brief Constructor
	
	Opens the LP-file to be read
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-06
	\param string Filename of the LP-file to be read
	\return void
	\sa
**/
		LPParser(string filename);
/** \brief Destructor
	
	Closes the LP-file that was read
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-06
	\return void
	\sa
**/
		~LPParser();
/** \brief Reads and parses the LP-file
	
	Reads the LP-file line-by-line and fills the properties objective, 
	constraints, bounds and variable.
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-06
	\return void
	\sa
**/
		void read();
/** \brief Collect variables of the LP
	
	Goes through objective, constraings and bounds and generates a list of all
	variable names that have been defined in the LP-file.
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-08
	\return void
	\sa
**/
		void collect_variables();
/** \brief Bring the LP to standard form
	
	 * Adds slack- and surplus-variables for inequalities
	 * Adds x+ and x- variables for unbounded variables
	 * Multiplies negative righthandsides by -1
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-08
	\return void
	\sa
**/
		void standardize();
/** \brief Create constraints out of bounds
	
	Replaces unbounded variables by x+ and x- variables. Updates
	the objective and the constraints accordingly. Adds constraints
	for all bounds.
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-08
	\return void
	\sa
**/
		void boundconstraints();
/** \brief Add slack and surplus variables
	
	Adds slack- and surplus-variables for inequalities such that all
	constraints become equal-constraints.
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-08
	\return void
	\sa
**/
		void slacksurplus();
/** \brief Assure that all righthandsides are positive
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-09
	\return void
	\sa
**/
		void positive_rhs();
/** \brief Print out the LP
	
	\author Christoph Tavan TU Berlin
	\param ostream& Outputstream to dump to
	\param bool Detailed- or readable output
	\date 2011-01-09
	\sa
**/
		void dump(Log& out, const bool& detailed = false);
/** \brief Print out the Objective
	
	\author Christoph Tavan TU Berlin
	\param ostream& Outputstream to dump to
	\param bool Detailed- or readable output
	\date 2011-01-09
	\sa
**/
		void dump_objective(Log& out, const bool& detailed = false);
/** \brief Print out the Constraints
	
	\author Christoph Tavan TU Berlin
	\param ostream& Outputstream to dump to
	\param bool Detailed- or readable output
	\date 2011-01-09
	\sa
**/
		void dump_constraints(Log& out, const bool& detailed = false);
/** \brief Print out the Bounds
	
	\author Christoph Tavan TU Berlin
	\param ostream& Outputstream to dump to
	\param bool Detailed- or readable output
	\date 2011-01-09
	\sa
**/
		void dump_bounds(Log& out, const bool& detailed = false);
};

#endif
