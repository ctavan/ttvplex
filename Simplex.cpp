#include "Simplex.h"

using namespace std;
using namespace cln;

// Constructor
Simplex::Simplex() {
}

void Simplex::init()
{
	Log log;

	b.push_back(4);
	b.push_back(2);
	b.push_back(3);
	b.push_back(6);
	log.vector(b, "b");
	
	A.push_back(std::vector<int>());
	A.push_back(std::vector<int>());
	A.push_back(std::vector<int>());
	A.push_back(std::vector<int>());
	A[0].push_back(0);
	A[0].push_back(1);
	A[0].push_back(2);
	A[0].push_back(3);
	A[1].push_back(4);
	A[1].push_back(5);
	A[1].push_back(6);
	A[1].push_back(7);
	A[2].push_back(8);
	A[2].push_back(9);
	A[2].push_back(10);
	A[2].push_back(11);
	A[3].push_back(12);
	A[3].push_back(13);
	A[3].push_back(14);
	A[3].push_back(15);
	log.matrix(A, "A");
}

void Simplex::phase2()
{
}
