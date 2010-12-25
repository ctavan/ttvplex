#include "Simplex.h"

using namespace std;
using namespace cln;

// Constructor
Simplex::Simplex() {
}

void Simplex::init()
{
	Log log;

	// Righthand side
	b.clear();
	b.push_back(1);
	b.push_back(3);
	b.push_back(4);
	log.vec(b, "b");

	// Matrix A
	A.clear();
	A.push_back(vector<cl_RA>());
	A.push_back(vector<cl_RA>());
	A.push_back(vector<cl_RA>());
	A[0].push_back(3);
	A[0].push_back(2);
	A[0].push_back(1);
	A[0].push_back(0);
	A[0].push_back(0);

	A[1].push_back(5);
	A[1].push_back(1);
	A[1].push_back(1);
	A[1].push_back(1);
	A[1].push_back(0);

	A[2].push_back(2);
	A[2].push_back(5);
	A[2].push_back(1);
	A[2].push_back(0);
	A[2].push_back(1);
	log.matrix(A, "A");

	// Cost coefficients c
	c.clear();
	c.push_back(1);
	c.push_back(1);
	c.push_back(1);
	c.push_back(1);
	c.push_back(1);
	log.vec(c, "c");

	// Carry Matrix
	CARRY.push_back(vector<cl_RA>());
	CARRY.push_back(vector<cl_RA>());
	CARRY.push_back(vector<cl_RA>());
	CARRY.push_back(vector<cl_RA>());

	CARRY[0].push_back(0);
	CARRY[0].push_back(0);
	CARRY[0].push_back(0);
	CARRY[0].push_back(0);

	CARRY[1].push_back(b[0]);
	CARRY[1].push_back(1);
	CARRY[1].push_back(0);
	CARRY[1].push_back(0);

	CARRY[2].push_back(b[1]);
	CARRY[2].push_back(0);
	CARRY[2].push_back(1);
	CARRY[2].push_back(0);

	CARRY[3].push_back(b[2]);
	CARRY[3].push_back(0);
	CARRY[3].push_back(0);
	CARRY[3].push_back(1);
	log.matrix(CARRY, "CARRY");
}

void Simplex::phase2()
{
}
