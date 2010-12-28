#include "main.h"

using namespace std;
using namespace cln;

int main ()
{
	lout << "Hello World!\n";

	// CLN-Test
	cl_RA rat = (cl_I)"21938129038091233132"/(cl_I)"2012390109283092183092183091283902183091283981209381209";
	lout << "Rational: " << rat << " Numerator: " << numerator(rat) << " Denom: " << denominator(rat)  << "\n";
	lout << "Denom + 5: " << denominator(rat)+5 << "\n";
	rat = (cl_I)3/(cl_I)9;
	lout << "3/9: " << rat << "\n";

	Simplex smp;
	smp.init();

	return 0;
}