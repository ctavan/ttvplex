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

	// std::cout << fibonacci(10) << std::endl;

	Simplex smp;
	smp.init();
	return 0;
}