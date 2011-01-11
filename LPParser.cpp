#include "LPParser.h"

using namespace std;

// Constructor
LPParser::LPParser(string filename)
{
	ldbg << "LPParser: Opening LP-file: " << filename << "\n";
	lpfile.open(filename.c_str(), ios::in);
	if (!lpfile.is_open())
	{
		lerr << "LPParser: couldn't open LP-File: " << filename << "\n";
		exit(EXIT_FAILURE);
	}
}

// Destructor
LPParser::~LPParser()
{
	lpfile.close();
}

void LPParser::read()
{
	string line;

	// Empty any previousely set data
	objective.elements.empty();

	section = SEC_START;

	// Variables needed, since constraints can be split over multiple lines
	int constraint_cur = -1; // Index of current constraint being processed
	bool constraint_rhs = false; // Is set to true, if the relation-symbol af a constraint has been passed

	while (lpfile.good() && section != SEC_END)
	{
		getline(lpfile, line);

		// Clean the input line
		ldbg << "LPParser: Parsing line: " << line << "\n";
		line = trim(line, false);
		ldbg << "LPParser: Cleaned line: " << line << "\n";

		// Ignore empty lines
		if (line.size() == 0)
		{
			ldbg << "LPParser: Ignoring empty line\n";
			continue;
		}

		// Evaluate the occurance of keywords that start a new section
		if (   line.find("maximize") == 0
			|| line.find("max") == 0 )
		{
			linf << "LPParser: Entering section OBJECTIVE\n";
			section = SEC_OBJECTIVE;
			// Set direction of the objective
			objective.direction = LPObjective::OBJ_MAX;
			continue;
		}
		if (   line.find("minimize") == 0
			|| line.find("min") == 0 )
		{
			linf << "LPParser: Entering section OBJECTIVE\n";
			section = SEC_OBJECTIVE;
			// Set direction of the objective
			objective.direction = LPObjective::OBJ_MIN;
			continue;
		}
		if (   line.find("subject to") == 0
			|| line.find("such that") == 0
			|| line.find("s.t.") == 0
			|| line.find("st.") == 0
			|| line.find("st") == 0 )
		{
			// Now enter CONSTRAINT section
			linf << "LPParser: Entering section CONSTRAINTS\n";
			section = SEC_CONSTRAINTS;
			continue;
		}
		if (   line.find("bounds") == 0
		 	|| line.find("bound") == 0 )
		{
			linf << "LPParser: Entering section BOUNDS\n";
			section = SEC_BOUNDS;
			continue;
		}
		if (   line.find("generals") == 0
		 	|| line.find("general") == 0
		 	|| line.find("gen") == 0 )
		{
			linf << "LPParser: Entering section GENERALS\n";
			section = SEC_GENERALS;
			continue;
		}
		if (   line.find("binaries") == 0
		 	|| line.find("binary") == 0
		 	|| line.find("bin") == 0 )
		{
			linf << "LPParser: Entering section BINARIES\n";
			section = SEC_BINARIES;
			continue;
		}
		if (   line.find("end") == 0 )
		{
			linf << "LPParser: Entering section END\n";
			section = SEC_END;
			continue;
		}

		// Now also remove all remaining whitespaces
		line = trim(line);
		ldbg << "LPParser: Entirely cleaned line: " << line << "\n";

		// Check if there we're in a named line (i.e. colon is present)
		string name("");
		size_t k;
		if ((k = line.find(":")) != string::npos)
		{
			name = line.substr(0,k);
			ldbg << "LPParser: Colon found at position " << k << " => Named line with name: " << name << "\n";
			line = line.substr(k+1);
		}
		ldbg << "LPParser: Remaining line without name: " << line << "\n";

		// Parse the rest
		vector<string> parts;		// Holding the parts of an expression
		bool constraint_finished;	// Is set to true, if the end of a constraint is reached
		LPConstraint constr;		// Holding a temporary copy of the constraint that is currently processed
		switch (section)
		{
			case SEC_START:
				break;

			case SEC_OBJECTIVE:
				// Set objective name
				if (name.size() > 0)
				{
					objective.name = name;
				}
				// Split the expression. The resulting array always has a + or - followed by a variable/coefficient
				// expression. +/- signs indicate the start of a new variable.
				parts = split_expression(line);
				trim(parts);
				ldbg.vec(parts, "Parts of the line");
				for (unsigned i = 0; i < parts.size(); i++)
				{
					// Parse variable/coefficient pairs and write them into objective.elements
					parse_varcoeff(parts[i], objective.elements);
				}
				break;

			case SEC_CONSTRAINTS:
				constraint_finished = false;
				// constraint_cur = -1 means we need to add a new constraint
				if (constraint_cur == -1)
				{
					constraints.push_back(constr);
					constraint_cur = constraints.size()-1;
				}
				// Get current constraint into variable for convenience
				constr = constraints[constraint_cur];
				// If name was given, use it
				if (name.size() > 0)
				{
					constr.name = name;
				}
				parts = split_expression(line);
				trim(parts);
				ldbg.vec(parts, "Parts of the line");
				for (unsigned i = 0; i < parts.size(); i++)
				{
					// If the relation has been reached set constraint_rhs = true
					if (parts[i].find_first_of("<>=") != string::npos)
					{
						// Set relation
						if (parts[i] == ">")
						{
							constr.relation = LPConstraint::REL_GE;
						}
						if (parts[i] == "<")
						{
							constr.relation = LPConstraint::REL_LE;
						}
						if (parts[i] == "=")
						{
							constr.relation = LPConstraint::REL_EQ;
						}

						// Initialize righthand side
						constr.rhs = 1;

						// Tell the rest of the loop that we're on the rhs now
						constraint_rhs = true;
					}
					// We're on the righthand side now
					else if (constraint_rhs)
					{
						// In case righthand side starts with a minus, set the coefficient
						if (parts[i] == "-")
						{
							constr.rhs = -1;
						}
						// In case righthand side starts with a plus, set the coefficient
						else if (parts[i] == "+")
						{
							constr.rhs = +1;
						}
						// Here we finally have the value of the RHS
						else
						{
							constr.rhs *= (mpq_class)parts[i];
							constraint_rhs = false;
							constraint_finished = true;
						}
					}
					// Here we're still on the lefthand side
					else
					{
						// Parse variable/coefficient pairs from lefthand side and write them into objective.elements
						parse_varcoeff(parts[i], constr.elements);
					}
				}
				// Update current constraint
				constraints[constraint_cur] = constr;
				// Set constraint_cur = -1 if a constraint has been finished, so that a new one is generated in the next step
				if (constraint_finished) {
					constraint_cur = -1;
				}
				break;

			case SEC_BOUNDS:
				parts = split_expression(line, "<>=");
				trim(parts);
				ldbg.vec(parts, "Parts of the line");
				int bound_cur = -1;
				string lower("");
				string upper("");
				int btype = 0;

				// Determine variable name
				// Bound given in 0 < x < 20 form
				if (parts.size() == 5)
				{
					name = parts[2];
					btype = 1;
				}
				// Bound given in 20 < x form, i.e. parts[0] is numeric
				if (parts.size() == 3 && parts[0].find_first_of("0123456789.") == 0)
				{
					name = parts[2];
					btype = 2;
				}
				// Bound given in x < 20, i.e. parts[2] is numeric
				else if (parts.size() == 3)
				{
					name = parts[0];
					btype = 3;
				}
				// Free variable: x FREE (transformed to xfree)
				if (parts.size() == 1)
				{
					// Variable name is just the string truncated by 4 chars ("free")
					name = parts[0].substr(0, parts[0].size()-4);
					btype = 4;
				}

				// Check if the bound has already been set
				for (unsigned i = 0; i < bounds.size(); i++)
				{
					if (bounds[i].name == name)
					{
						bound_cur = i;
						break;
					}
				}
				if (bound_cur == -1)
				{
					LPBound bound;
					bounds.push_back(bound);
					bound_cur = bounds.size()-1;
				}
				bounds[bound_cur].name = name;
				
				switch(btype)
				{
					// 0 < x < 20 form
					case 1:
						if (parts[0].find("inf") != string::npos)
						{
							bounds[bound_cur].lower = 0;
							bounds[bound_cur].lower_unbound = true;
						}
						else
						{
							bounds[bound_cur].lower = parts[0];
							bounds[bound_cur].lower_unbound = false;
						}
						if (parts[4].find("inf") != string::npos)
						{
							bounds[bound_cur].upper = 0;
							bounds[bound_cur].upper_unbound = true;
						}
						else
						{
							bounds[bound_cur].upper = parts[4];
							bounds[bound_cur].upper_unbound = false;
						}
						break;

					// 20 < x
					case 2:
						if (parts[0].find("inf") != string::npos)
						{
							bounds[bound_cur].lower = 0;
							bounds[bound_cur].lower_unbound = true;
						}
						else
						{
							bounds[bound_cur].lower = parts[0];
							bounds[bound_cur].lower_unbound = false;
						}
						break;

					// x < 20
					case 3:
						if (parts[2].find("inf") != string::npos)
						{
							bounds[bound_cur].upper = 0;
							bounds[bound_cur].upper_unbound = true;
						}
						else
						{
							bounds[bound_cur].upper = parts[2];
							bounds[bound_cur].upper_unbound = false;
						}
						break;

					case 4:
						bounds[bound_cur].upper = 0;
						bounds[bound_cur].upper_unbound = true;
						bounds[bound_cur].lower = 0;
						bounds[bound_cur].lower_unbound = true;
						break;
				}
				
				break;
		}
	}

	// Dump the lp
	linf << "LPParser: LP as read from the input file:\n";
	dump(linf);

	// Finally collect all variables of the system
	collect_variables();
}

void LPParser::collect_variables()
{
	ldbg << "LPParser: collecting variables...\n";
	ldbg << "LPParser:   ... from objective\n";
	for (unsigned j = 0; j < objective.elements.size(); j++)
	{
		variables.add(objective.elements[j].name);
	}
	ldbg << "LPParser:   ... from constraints\n";
	for (unsigned i = 0; i < constraints.size(); i++)
	{
		for (unsigned j = 0; j < constraints[i].elements.size(); j++)
		{
			variables.add(constraints[i].elements[j].name);
		}
	}
	ldbg << "LPParser:   ... from bounds\n";
	for (unsigned i = 0; i < bounds.size(); i++)
	{
		variables.add(bounds[i].name);
	}
	linf << "LPParser: Variables in the system:\n";
	variables.dump(linf);
}

void LPParser::standardize()
{
	linf << "LPParser: Bringing the LP to standard form\n";
	// Transform MAX to MIN
	if (objective.direction == LPObjective::OBJ_MAX)
	{
		for (unsigned i = 0; i < objective.elements.size(); i++)
		{
			objective.elements[i].coeff *= -1;
		}
	}
	boundconstraints();
	slacksurplus();
	positive_rhs();
}

void LPParser::boundconstraints()
{
	ldbg << "LPParser: Checking negative and unbound variables\n";
	for (unsigned i = 0; i < bounds.size(); i++)
	{
		// We don't have to do anything, if the lower bound is 0 and there is no upper bound.
		if (!bounds[i].lower_unbound && bounds[i].lower == 0 && bounds[i].upper_unbound)
		{
			continue;
		}
		// Variable x bounded from. Since it has a finite upper bound -> Add constraint of form x < 20
		if (!bounds[i].lower_unbound && !bounds[i].upper_unbound)
		{
			LPConstraint constr;
			constr.name = "UpperBound_"+bounds[i].name;
			constr.relation = LPConstraint::REL_LE;
			constr.rhs = bounds[i].upper;
			LPVariable var;
			var.name = bounds[i].name;
			var.coeff = 1;
			constr.elements.push_back(var);
			constraints.push_back(constr);
		}
		// Variable is not unbounded from below, but has a nonzero lower bound
		if (!bounds[i].lower_unbound && bounds[i].lower != 0)
		{
			mpq_class lower = bounds[i].lower;
			string oldname = bounds[i].name;
			string newname = variables.replaceBounded(bounds[i].name);
			ldbg << "Variable '" << oldname << "' has nonzero lower bound: " << lower << "\n";
			ldbg << "Adding new variable " << newname << " = " << oldname << " + " << -1*lower << "\n";

			// Replace variable in the objective, and account for the offset
			for (unsigned j = 0; j < objective.elements.size(); j++)
			{
				if (objective.elements[j].name != oldname)
				{
					continue;
				}
				ldbg << "Oldcoeff " << objective.elements[j].coeff << " bounds lower" << "\n";
				objective.elements[j].name = newname;
				objective.offset += objective.elements[j].coeff*lower;
			}

			// Replace variable in the constraints
			for (unsigned j = 0; j < constraints.size(); j++)
			{
				for (unsigned k = 0; k < constraints[j].elements.size(); k++)
				{
					if (constraints[j].elements[k].name != oldname)
					{
						continue;
					}
					ldbg << "Found variable " << oldname << " in this constraint, adjusting rhs.\n";
					// b_x = x - lower => x = b_x + lower
					// constraints[j].dump(ldbg);
					constraints[j].elements[k].name = newname;
					constraints[j].rhs -= constraints[j].elements[k].coeff*lower;
					// constraints[j].dump(ldbg);
				}
			}
		}
		// Variable is free
		if (bounds[i].lower_unbound && bounds[i].upper_unbound)
		{
			ldbg << "Variable " << bounds[i].name << " is unbounded.\n";

			// Split variable
			variables.splitPlusMinus(bounds[i].name);

			string oldname = bounds[i].name;
			string pname = variables.split_prefix_p + oldname;
			string mname = variables.split_prefix_m + oldname;

			// Replace variable in the objective, and account for the offset
			for (unsigned j = 0; j < objective.elements.size(); j++)
			{
				if (objective.elements[j].name != oldname)
				{
					continue;
				}
				ldbg << "Oldcoeff " << objective.elements[j].coeff << " bounds lower" << "\n";
				objective.elements[j].name = pname;
				LPVariable mvar;
				mvar.name = mname;
				mvar.coeff = -1*objective.elements[j].coeff;
				objective.elements.push_back(mvar);
			}

			// Replace variable in the constraints
			for (unsigned j = 0; j < constraints.size(); j++)
			{
				for (unsigned k = 0; k < constraints[j].elements.size(); k++)
				{
					if (constraints[j].elements[k].name != oldname)
					{
						continue;
					}
					ldbg << "Found variable " << oldname << " in this constraint, replacing by split variable.\n";
					// b_x = x - lower => x = b_x + lower
					// constraints[j].dump(ldbg);
					constraints[j].elements[k].name = pname;
					LPVariable mvar;
					mvar.name = mname;
					mvar.coeff = -1*constraints[j].elements[k].coeff;
					constraints[j].elements.push_back(mvar);
					// constraints[j].dump(ldbg);
				}
			}
		}
	}
}

void LPParser::slacksurplus()
{
	ldbg << "LPParser: Introducing slack and surplus variables\n";
	for (unsigned i = 0; i < constraints.size(); i++)
	{
		// Nothing to be done, if constraint is an equals relation
		if (constraints[i].relation == LPConstraint::REL_EQ)
		{
			continue;
		}
		// Get a unique name for the slack/surplus variable
		string name = variables.addSlackSurplus();
		// Create a new variable and add it to the constraint
		LPVariable slacksurplus;
		slacksurplus.name = name;
		// Set coefficient to -1 for surplus variables
		if (constraints[i].relation == LPConstraint::REL_GE)
		{
			slacksurplus.coeff = -1;
		}
		constraints[i].elements.push_back(slacksurplus);
		// Since we've now added slack/surplus variable, we get an equal-constraint!
		constraints[i].relation = LPConstraint::REL_EQ;
	}
}

void LPParser::positive_rhs()
{
	ldbg << "LPParser: Assuring positive righthandsides\n";
	for (unsigned i = 0; i < constraints.size(); i++)
	{
		// If rhs is positive, everything is ok
		if (constraints[i].rhs >= 0)
		{
			continue;
		}
		// Otherwise multiply all coefficients and the rhs by -1
		constraints[i].rhs *= -1;
		for (unsigned j = 0; j < constraints[i].elements.size(); j++)
		{
			constraints[i].elements[j].coeff *= -1;
		}
	}
}

string LPParser::trim(string line, const bool& strip_spaces)
{
	// If line is empty, nothing has to be cleaned
	if (line.size() == 0) {
		return line;
	}

	// Size variable
	size_t n;

	// Strip comments
	n = line.find_first_of("\\");
	// First character of line is comment-character '\' so we just forget about that line
	if (n == 0)
	{
		ldbg << "LPParser: Comment starts at " << n << " => forgetting about the whole line\n";
		return "";
	}
	// If there was a comment in that line, throw away everything after the first occurance of '\'
	if (n != string::npos)
	{
		ldbg << "LPParser: Comment starts at " << n << "\n";
		line = line.substr(0, n);
	}

	// Clean line, @see http://www2.hawaii.edu/~wes/ICS212/Notes/CPPStrings.html
	// Replace tabs by whitespaces
	while((n = line.find('\t')) != string::npos)
	{
		line[n] = ' ';
	}
	// remove leading and trailing spaces in line
	// n is location of first letter, k is location of last
	size_t k;
	n = line.find_first_not_of(" ");
	k = line.find_last_not_of(" ");
	line = line.substr(n, k-n+1); //keep n-k+1 chars

	// Transform everything to lowercase
	transform(line.begin(), line.end(), line.begin(), ::tolower);

	// Remove multiple whitespaces
	while((k = line.find("  ")) != string::npos)
	{
		line.erase(k, 1);
	}

	// Remove all spaces
	if (strip_spaces)
	{
		line.erase(remove(line.begin(), line.end(), ' '), line.end());
	}

	return line;
}
void LPParser::trim(vector<string>& lines) {
	for (unsigned i = 0; i < lines.size(); i++)
	{
		lines[i] = trim(lines[i]);
	}
}

vector<string> LPParser::split_expression(const string& strbase, const string& delimiters)
{
	const string relsyms = "<>=";

	vector<string> tokens;

	// Copy the string to be split
	string str = strbase;

	// If <= or >= are used, strip the =
	if (string::npos != str.find_first_of("=") && (string::npos != str.find_first_of(">") || string::npos != str.find_first_of("<")))
	{
		str.erase( remove( str.begin(), str.end(), '=' ), str.end() );
	}

	ldbg << "LPParser: Splitting string: " << str << "\n";

	// Tokens are assembled character by character
	string token("");

	for(unsigned i = 0; i < str.size(); i++)
	{
		if (str.substr(i, 1).find_first_of(delimiters) != string::npos || str.substr(i, 1).find_first_of(relsyms) != string::npos)
		{
			if (i > 0 && token.size() > 0)
			{
				ldbg << "LPParser: => token finished: " << token << "\n";
				tokens.push_back(token);
				token = "";
			}
			ldbg << "LPParser: => relation symbol/operator added: " << str.substr(i, 1) << "\n";
			tokens.push_back(str.substr(i, 1));
		}
		if (   str.substr(i, 1).find_first_not_of(delimiters) != string::npos 
			&& str.substr(i, 1).find_first_not_of(relsyms) != string::npos)
		{
			token += str.substr(i, 1);
		}
		// If line ends with a variable or coefficient, add that to the tokens
		if (i == str.size()-1 && token.size() > 0)
		{
			tokens.push_back(token);
		}
	}
	return tokens;
}

void LPParser::parse_varcoeff(string str, vector<LPVariable>& elements)
{
	LPVariable var;
	ldbg << "LPParser: Parsing part: " << str << "\n";

	// Get latest variable
	if (elements.size() > 0)
	{
		var = elements[elements.size()-1];
	}

	// Add a new variable, if current part is plus or minus
	if (str == "-" || str == "+")
	{
		var.coeff = 1;
		if (str == "-")
		{
			var.coeff = -1;
		}
		elements.push_back(var);
		return;
	}
	// Fill the variable, that has been added in the previous step, with the correct values.
	// The variable has been added to the end of objective.elements in the step before.

	// Temporary variable to hold the variable coefficient
	string varcoeff("");
	// So find the first non-numeric occurance. This is where the coefficient ends and where the
	// variable name starts.
	size_t k = str.find_first_not_of("0123456789.");
	// If no variable name was given in str, this means coeafficient and variable name
	// have been separated by a newline in the lp-file and we're just processing the coefficient
	if (k == string::npos)
	{
		varcoeff = trim(str);
	}
	// If only the variable name was given but no coefficient, just use this one
	else if (k == 0)
	{
		var.name = trim(str);
	}
	// The third case is that the coefficient is followed by the variable name
	else
	{
		varcoeff = trim(str.substr(0, k));
		var.name = trim(str.substr(k));
	}

	if (varcoeff.size() > 0)
	{
		var.coeff = var.coeff*(mpq_class)atoi(varcoeff.c_str());
	}
	if (elements.size() > 0)
	{
		elements[elements.size()-1] = var;
	}
	else
	{
		elements.push_back(var);
	}
	ldbg << "LPParser: Generated variable '" << var.name << "' with coefficient '" << var.coeff << "'\n";
}

void LPParser::dump_objective(Log& out, const bool& detailed)
{
	out << "Objective:\n";
	objective.dump(out, detailed);
}

void LPParser::dump_constraints(Log& out, const bool& detailed)
{
	out << "Constraints:\n";
	for (unsigned i = 0; i < constraints.size(); i++)
	{
		constraints[i].dump(out, detailed);
	}	
}

void LPParser::dump_bounds(Log& out, const bool& detailed)
{
	out << "Bounds:\n";
	for (unsigned i = 0; i < bounds.size(); i++)
	{
		bounds[i].dump(out, detailed);
	}
}

void LPParser::dump(Log& out, const bool& detailed)
{
	dump_objective(out, detailed);
	dump_constraints(out, detailed);
	dump_bounds(out, detailed);
}