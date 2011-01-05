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

	section = SEC_START;

	while (lpfile.good() && section != SEC_END)
	{
		getline(lpfile, line);

		// Clean the input line
		ldbg << "LPParser: Parsing line: " << line << "\n";
		line = clean_line(line);
		ldbg << "LPParser: Cleaned line: " << line << "\n";

		// Ignore empty lines
		if (line.size() == 0) {
			ldbg << "LPParser: Ignoring empty line\n";
			continue;
		}

		// Evaluate the occurance of keywords that start a new section
		if (   line.find("maximize") != string::npos
			|| line.find("max") != string::npos
			|| line.find("minimize") != string::npos
			|| line.find("min") != string::npos ) {
			ldbg << "LPParser: Entering section OBJECTIVE\n";
			section = SEC_OBJECTIVE;
			continue;
		}
		if (   line.find("subject to") != string::npos
			|| line.find("such that") != string::npos
			|| line.find("s.t.") != string::npos
			|| line.find("st.") != string::npos
			|| line.find("st") != string::npos ) {
			ldbg << "LPParser: Entering section CONSTRAINTS\n";
			section = SEC_CONSTRAINTS;
			continue;
		}
		if (   line.find("bounds") != string::npos
		 	|| line.find("bound") != string::npos ) {
			ldbg << "LPParser: Entering section BOUNDS\n";
			section = SEC_BOUNDS;
			continue;
		}
		if (   line.find("end") != string::npos ) {
			ldbg << "LPParser: Entering section END\n";
			section = SEC_END;
			continue;
		}
	}
}

string LPParser::clean_line(string line)
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