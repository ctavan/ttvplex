#include "main.h"

using namespace std;
using namespace cln;

// Returns F_n, computed as the nearest integer to
// ((1+sqrt(5))/2)^n/sqrt(5). Assume n>=0.
const cl_I fibonacci (int n)
{
	// Need a precision of ((1+sqrt(5))/2)^-n.
	float_format_t prec = float_format((int)(0.208987641*n+5));
	cl_R sqrt5 = sqrt(cl_float(5,prec));
	cl_R phi = (1+sqrt5)/2;
	return round1( expt(phi,n)/sqrt5 );
}

int main ()
{
	std::cout << "Hello World!" << std::endl;

	WARN("Hello %s %d\n", "World", 1);
	INFO("Hello %s %d\n", "World", 2);
	DEBUG("Hello %s %d %s\n", "World", 3, "test");

	int n = 10;
	{ CL_TIMING;
		cout << "fib(" << n << ") = ";
		for (int rep = 10; rep > 0; rep--)
			fibonacci(n);
		cout << fibonacci(n) << endl;
	}

	cl_RA rat = (cl_I)"21938129038091233132"/(cl_I)"2012390109283092183092183091283902183091283981209381209";
	cout << "Rational: " << rat << " Numerator: " << numerator(rat) << " Denom: " << denominator(rat)  << endl;
	cout << "Denom + 5: " << denominator(rat)+5 << endl;
	rat = (cl_I)3/(cl_I)9;
	cout << "3/9: " << rat << endl;

	Simplex smp;
	smp.init();
	return 0;
}