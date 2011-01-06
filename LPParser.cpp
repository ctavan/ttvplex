#include "LPParser.h"

using namespace std;
using namespace cln;

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

	// Index of current constraint being processed
	int cur_constraint = -1;

	while (lpfile.good() && section != SEC_END)
	{
		getline(lpfile, line);

		// Clean the input line
		ldbg << "LPParser: Parsing line: " << line << "\n";
		line = trim(line);
		ldbg << "LPParser: Cleaned line: " << line << "\n";

		// Ignore empty lines
		if (line.size() == 0)
		{
			ldbg << "LPParser: Ignoring empty line\n";
			continue;
		}

		// Evaluate the occurance of keywords that start a new section
		if (   line.find("maximize") != string::npos
			|| line.find("max") != string::npos )
		{
			linf << "LPParser: Entering section OBJECTIVE\n";
			section = SEC_OBJECTIVE;
			// Set direction of the objective
			objective.direction = OBJ_MAX;
			continue;
		}
		if (   line.find("minimize") != string::npos
			|| line.find("min") != string::npos )
		{
			linf << "LPParser: Entering section OBJECTIVE\n";
			section = SEC_OBJECTIVE;
			// Set direction of the objective
			objective.direction = OBJ_MIN;
			continue;
		}
		if (   line.find("subject to") != string::npos
			|| line.find("such that") != string::npos
			|| line.find("s.t.") != string::npos
			|| line.find("st.") != string::npos
			|| line.find("st") != string::npos )
		{
			// Now enter CONSTRAINT section
			linf << "LPParser: Entering section CONSTRAINTS\n";
			section = SEC_CONSTRAINTS;
			continue;
		}
		if (   line.find("bounds") != string::npos
		 	|| line.find("bound") != string::npos )
		{
			linf << "LPParser: Entering section BOUNDS\n";
			section = SEC_BOUNDS;
			continue;
		}
		if (   line.find("end") != string::npos )
		{
			linf << "LPParser: Entering section END\n";
			section = SEC_END;
			continue;
		}

		// Check if there we're in a named line (i.e. colon is present)
		string name("");
		size_t k;
		if ((k = line.find(":")) != string::npos)
		{
			name = trim(line.substr(0,k));
			ldbg << "LPParser: Colon found at position " << k << " => Named line with name: " << name << "\n";
			line = trim(line.substr(k+1));
		}
		line.erase( remove( line.begin(), line.end(), ' ' ), line.end() );
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
				// cur_constraint = -1 means we need to add a new constraint
				if (cur_constraint == -1)
				{
					constraints.push_back(constr);
					cur_constraint = constraints.size()-1;
				}
				// Get current constraint into variable for convenience
				constr = constraints[cur_constraint];
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
					// If the relation has been reached we stop parsing more variables
					if (parts[i] == ">" || parts[i] == "<" || parts[i] == "=")
					{
						if (parts[i] == ">")
						{
							constr.relation = REL_GE;
						}
						if (parts[i] == "<")
						{
							constr.relation = REL_LE;
						}
						if (parts[i] == "=")
						{
							constr.relation = REL_EQ;
						}
						constr.rhs = parts[i+2];
						if (parts[i+1] == "-")
						{
							constr.rhs *= -1;
						}
						constraint_finished = true;
						break;
					}
					// Parse variable/coefficient pairs and write them into objective.elements
					parse_varcoeff(parts[i], constr.elements);
				}
				// Update current constraint
				constraints[cur_constraint] = constr;
				// Set cur_constraint = -1 if a constraint has been finished, so that a new one is generated in the next step
				if (constraint_finished) {
					cur_constraint = -1;
				}
				break;

			case SEC_BOUNDS:
				parts = split_expression(line);
				trim(parts);
				ldbg.vec(parts, "Parts of the line");
				break;
		}
	}
	exit(0);
	linf << "Objective:\n";
	objective.dump();
	linf << "Constraints:\n";
	for (unsigned i = 0; i < constraints.size(); i++)
	{
		constraints[i].dump();
	}
}

string LPParser::trim(string line)
{
	// If line is empty, nothing has to be cleande
	if (line.size() == 0) {
		return line;
	}

	// Sime size variables
	size_t n, k;

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

	return line;
}
void LPParser::trim(vector<string>& lines) {
	for (unsigned i = 0; i < lines.size(); i++)
	{
		lines[i] = trim(lines[i]);
	}
}

vector<string> LPParser::split_expression(const string& strbase)
{
	const string delimiters = "+-";
	const string relsyms = "<>=";

	vector<string> tokens;

	// Copy the string to be split
	string str = strbase;

	// If <= or >= are used, strip the =
	if (string::npos != str.find_first_of("=") && (string::npos != str.find_first_of(">") || string::npos != str.find_first_of("<")))
	{
		str.erase( remove( str.begin(), str.end(), '=' ), str.end() );
	}
	ldbg << "splitting string: " << str << "\n";

	string token("");

	for(unsigned i = 0; i < str.size(); i++)
	{
		if (str.substr(i, 1).find_first_of(delimiters) != string::npos || str.substr(i, 1).find_first_of(relsyms) != string::npos)
		{
			if (i > 0 && token.size() > 0)
			{
				ldbg << "=> token finished: " << token << "\n";
				tokens.push_back(token);
				token = "";
			}
			ldbg << "=> relation symbol/operator added: " << str.substr(i, 1) << "\n";
			tokens.push_back(str.substr(i, 1));
		}
		if (   str.substr(i, 1).find_first_not_of(delimiters) != string::npos 
			&& str.substr(i, 1).find_first_not_of(relsyms) != string::npos)
		{
			token += str.substr(i, 1);
		}
		if (i == str.size()-1)
		{
			tokens.push_back(token);
		}
	}
	return tokens;

	// If the string doesn't start with a delimiter or a relation symbol,
	// it must start with a coefficient prepend a +
	if (str.find_first_of(delimiters) != 0 && str.find_first_of(relsyms) != 0)
	{
		str = "+" + str;
	}



	// First delimiter
	string::size_type cur	  = str.find_first_of(delimiters);
	// Next delimiter
	string::size_type next	  = str.find_first_of(delimiters, cur+1);
	// First non-delimiting char
	string::size_type tokenend = str.find_last_not_of(delimiters, next);

	bool finished = false;
	bool lastround = false;
	while (!finished)
	{
		ldbg << "cur " << cur << " next " << next << " last " << tokenend << "\n";
		// Extract token (including delimiter)
		string token = str.substr(cur, tokenend - cur + 1);
		// Get the sign (i.e. the 1-character delimiter)
		string sign  = token.substr(0, 1);
		// Store sign
		tokens.push_back(sign);
		// Now strip the delimiter
		token = token.substr(1);
		ldbg << "token: " << token << "\n";
		// Check if remaining token contains relation symbols
		string::size_type rel = token.find_first_of(relsyms);
		// Found a token with relation symbol, add it to the vector.
		if (string::npos != rel)
		{
			ldbg << "Relation symbol in token: " << rel << "\n";
			string relation = token.substr(rel, 1);
			token = token.substr(0, rel);
			ldbg << "token: " << token << "\n";
			ldbg << "rel: " << relation << "\n";
			tokens.push_back(token);
			tokens.push_back(relation);
		}
		else
		{
			// Found a token without relation symbol, add it to the vector.
			tokens.push_back(token);
		}
		ldbg << "token " << token << "\n";
		// First delimiter
		cur	 = str.find_first_of(delimiters, next);
		// Next delimiter
		next = str.find_first_of(delimiters, cur+1);
		// First non-delimiting char
		tokenend = str.find_last_not_of(delimiters, next);
		ldbg << "cur " << cur << " next " << next << " last " << tokenend << "\n";

		if (lastround)
		{
			finished = true;
		}
	}

	return tokens;

	// First delimiter
	string::size_type pos	  = str.find_first_of(delimiters);
	// First non-delimiting char
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);

	while (string::npos != pos || string::npos != lastPos)
	{
		string token = str.substr(lastPos, pos - lastPos);
		ldbg << "found token: " << token << "\n";
		string::size_type rel = token.find_first_of(relsyms);
		// Found a token with relation symbol, add it to the vector.
		if (string::npos != rel)
		{
			ldbg << "Relation symbol in token: " << rel << "\n";
			string op = token.substr(rel, 1);
			// Translate => and >= to >
			if (op == "=" && token.substr(rel+1, 1) == ">")
			{
				op = ">";
			}
			// Translate =< and <= to <
			if (op == "=" && token.substr(rel+1, 1) == "<")
			{
				op = "<";
			}
			token = token.substr(0, rel);
			ldbg << "token: " << token << "\n";
			ldbg << "rel: " << op << "\n";
			tokens.push_back(token);
			tokens.push_back(op);
		}
		else
		{
			// Found a token without relation symbol, add it to the vector.
			tokens.push_back(token);
		}
		// Also add the separator
		if (string::npos != pos) {
			string op = str.substr(pos, 1);
			ldbg << "operator: " << op << "\n";
			tokens.push_back(op);
			// 			// Translate => and >= to >
			// 			if (op == "=" && str.substr(pos+1, 1) == ">")
			// 			{
			// 				op = ">";
			// 			}
			// 			// Translate =< and <= to <
			// 			if (op == "=" && str.substr(pos+1, 1) == "<")
			// 			{
			// 				op = "<";
			// 			}
			// 			tokens.push_back(op);
			// if (op == "<" || op == ">" || op == "=")
			// {
			// 	string::size_type nextOp = str.find_first_of("+-", pos);
			// 	string::size_type nextVar = str.find_first_not_of("+-><= ", pos);
			// 	ldbg << "nextOp " << nextOp << "nextVar " << nextVar << "\n";
			// 	// In case  there is no + or - following the >/</= add a + sign
			// 	if (nextVar < nextOp)
			// 	{
			// 		ldbg << "Added extra +\n";
			// 		tokens.push_back("+");
			// 	}
			// 	else
			// 	{
			// 		ldbg << "Next operator is: " << str.substr(nextOp, 1) << "\n";
			// 		tokens.push_back(str.substr(nextOp, 1));
			// 		lastPos = nextVar;
			// 		pos = nextOp;
			// 	}
			// }
		}
		// Skip delimiters.	 Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
	return tokens;
}

void LPParser::parse_varcoeff(string str, vector<LPVariable>& elements)
{
	LPVariable var;
	ldbg << "LPParser: Parsing part: " << str << "\n";
	// Add a new variable
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
	var = elements[elements.size()-1];
	size_t k = str.find_first_not_of("0123456789.");
	var.name = trim(str.substr(k));
	string varcoeff = trim(str.substr(0, k));
	if (varcoeff.size() > 0)
	{
		var.coeff = var.coeff*(mpq_class)atoi(varcoeff.c_str());
	}
	ldbg << "LPParser: Generated variable '" << var.name << "' with coefficient '" << var.coeff << "'\n";
	elements[elements.size()-1] = var;
}