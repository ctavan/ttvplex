#include "main.h"

using namespace std;

int main(int argc, char** argv)
{
	// Some variables that will be filled with values given through the commandline
	string input;	// Inputfilename
	bool display_problem = false;	// Whether to display problem again
	bool display_objective = false;	// Whether to display objective
	bool display_variables = false;	// Whether to display variables
	bool optimize = false;	// Whether to optimize

	// Handle commandline arguments using the nice TCLAP library
	// @see http://tclap.sourceforge.net/
	//
	// Wrap everything in a try block. Do this every time, 
	// because exceptions will be thrown for problems.
	try {
		// ============ Define available arguments ============

		// Define the command line object, and insert a message
		// that describes the program. The "Command description message" 
		// is printed last in the help text. The second argument is the 
		// delimiter (usually space) and the last one is the version number. 
		// The CmdLine object parses the argv array based on the Arg objects
		// that it contains. 
		TCLAP::CmdLine cmd("TTVPlex - Revised Simplex Algorithm", ' ', "0.1");

		// Define a value argument and add it to the command line.
		// A value arg defines a flag and a type of value that it expects,
		// such as "-n Bishop".
		TCLAP::ValueArg<int> verbosityArg("v","verbosity","Verbosity of output (0 to 3)", false, 1, "int");
		// Add the argument nameArg to the CmdLine object. The CmdLine object
		// uses this Arg to parse the command line.
		cmd.add( verbosityArg );


		TCLAP::ValueArg<string> inputArg("i","input","Input LP file", true, "", "string");
		cmd.add( inputArg );

		TCLAP::ValueArg<int> maxitArg("m","maxit","Maximum number of iterations", false, 0, "int");
		cmd.add( maxitArg );

		TCLAP::SwitchArg display_problemArg("p","problem","Display problem?", false);
		cmd.add( display_problemArg );

		TCLAP::SwitchArg display_objectiveArg("o","objective","Display objective?", false);
		cmd.add( display_objectiveArg );

		TCLAP::SwitchArg display_variablesArg("w","variables","Display variables?", false);
		cmd.add( display_variablesArg );

		TCLAP::SwitchArg optimizeArg("x","optimize","Execute optimization?", false);
		cmd.add( optimizeArg );

		// Parse the argv array.
		cmd.parse( argc, argv );


		// ============ Postprocess some of the arguments ============

		// Get the value parsed by each arg. 
		int verbosity = verbosityArg.getValue();

		lout << "Verbosity of output: " << verbosity << "\n";
		if (verbosity < 3) {
			ldbg.level = 0;
		}
		if (verbosity < 2) {
			linf.level = 0;
		}
		if (verbosity < 1) {
			lout.level = 0;
			lerr.level = 0;
		}

		input = inputArg.getValue();
		max_iterations = maxitArg.getValue();

		display_problem = display_problemArg.getValue();
		display_objective = display_objectiveArg.getValue();
		display_variables = display_variablesArg.getValue();

		optimize = optimizeArg.getValue();
	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
		cerr << "error: " << e.error() << " for arg " << e.argId() << endl; 
		exit(EXIT_FAILURE);
	}


	LPParser lp(input);
	lp.read();
	if (display_problem)
	{
		lout << "===================================================\n";
		lout << "LP read from file: '" << input << "'\n";
		lp.dump(lout);
		lout << "\n";
	}
	lp.standardize();
	if (display_problem)
	{
		lout << "===================================================\n";
		lout << "LP now in standard form:\n";
		lp.dump(lout);
		lout << "\n";
	}

	Simplex smp(lp);
	smp.init();
	if (optimize)
	{
		smp.optimize();
	}
	if (display_objective)
	{
		lout << "\n";
		smp.objective();
	}
	if (display_variables)
	{
		lout << "\n";
		smp.variables();
	}

	exit(EXIT_SUCCESS);

	return 0;
}