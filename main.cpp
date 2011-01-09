#include "main.h"

using namespace std;

int main(int argc, char** argv)
{
	// Some variables that will be filled with values given through the commandline
	string input;	// Inputfilename
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
	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
		cerr << "error: " << e.error() << " for arg " << e.argId() << endl; 
	}


	LPParser lp(input);
	lp.read();
	lp.standardize();

	// Simplex smp;
	// smp.init(lp);

	exit(EXIT_SUCCESS);

	return 0;
}