#include "Simplex.h"

using namespace std;
using namespace cln;

// Constructor
Simplex::Simplex() {
}

void Simplex::init()
{
	// Index set for basis columns
	basis.clear();
	basis.push_back(1);
	basis.push_back(2);
	basis.push_back(3);

	// Righthand side
	b.clear();
	b.push_back(1);
	b.push_back(3);
	b.push_back(4);
	Log::vec(b, "b");

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
	Log::matrix(A, "A");

	// Cost coefficients c
	c.clear();
	c.push_back(1);
	c.push_back(1);
	c.push_back(1);
	c.push_back(1);
	c.push_back(1);
	Log::vec(c, "c");


	// Carry Matrix
	CARRY.clear();
	CARRY.push_back(vector<cl_RA>());
	CARRY.push_back(vector<cl_RA>());
	CARRY.push_back(vector<cl_RA>());
	CARRY.push_back(vector<cl_RA>());
	CARRY.push_back(vector<cl_RA>());

	int i = 0;
	CARRY[i].push_back(0);
	CARRY[i].push_back(0);
	CARRY[i].push_back(0);
	CARRY[i].push_back(0);
	CARRY[i].push_back(1);
	CARRY[i].push_back(1);
	CARRY[i].push_back(1);
	CARRY[i].push_back(1);
	CARRY[i].push_back(1);

	i++;
	CARRY[i].push_back(0);
	CARRY[i].push_back(1);
	CARRY[i].push_back(1);
	CARRY[i].push_back(1);
	CARRY[i].push_back(0);
	CARRY[i].push_back(0);
	CARRY[i].push_back(0);
	CARRY[i].push_back(0);
	CARRY[i].push_back(0);

	i++;
	CARRY[i].push_back(b[0]);
	CARRY[i].push_back(1);
	CARRY[i].push_back(0);
	CARRY[i].push_back(0);
	CARRY[i].push_back(3);
	CARRY[i].push_back(2);
	CARRY[i].push_back(1);
	CARRY[i].push_back(0);
	CARRY[i].push_back(0);

	i++;
	CARRY[i].push_back(b[1]);
	CARRY[i].push_back(0);
	CARRY[i].push_back(1);
	CARRY[i].push_back(0);
	CARRY[i].push_back(5);
	CARRY[i].push_back(1);
	CARRY[i].push_back(1);
	CARRY[i].push_back(1);
	CARRY[i].push_back(0);

	i++;
	CARRY[i].push_back(b[2]);
	CARRY[i].push_back(0);
	CARRY[i].push_back(0);
	CARRY[i].push_back(1);
	CARRY[i].push_back(2);
	CARRY[i].push_back(5);
	CARRY[i].push_back(1);
	CARRY[i].push_back(0);
	CARRY[i].push_back(1);
	Log::matrix(CARRY, "CARRY");

	Matrix::rowSubtract(CARRY, 1, 2);
	Matrix::rowSubtract(CARRY, 1, 3);
	Matrix::rowSubtract(CARRY, 1, 4);
	Log::matrix(CARRY, "CARRY");
	Matrix::pivot(CARRY, CARRY, 2, 4);
	Log::matrix(CARRY, "CARRY");
	Matrix::pivot(CARRY, CARRY, 2, 5);
	Log::matrix(CARRY, "CARRY");
	Matrix::pivot(CARRY, CARRY, 3, 7);
	Log::matrix(CARRY, "CARRY");
	Matrix::pivot(CARRY, CARRY, 4, 8);
	Log::matrix(CARRY, "CARRY");
	Log::message("=================");

	// Carry-Matrix
	CARRY.clear();
	CARRY.push_back(vector<cl_RA>());
	CARRY.push_back(vector<cl_RA>());
	CARRY.push_back(vector<cl_RA>());
	CARRY.push_back(vector<cl_RA>());

	i = 0;
	CARRY[i].push_back(-b[0]-b[1]-b[2]);
	CARRY[i].push_back(0);
	CARRY[i].push_back(0);
	CARRY[i].push_back(0);

	i++;
	CARRY[i].push_back(b[0]);
	CARRY[i].push_back(1);
	CARRY[i].push_back(0);
	CARRY[i].push_back(0);

	i++;
	CARRY[i].push_back(b[1]);
	CARRY[i].push_back(0);
	CARRY[i].push_back(1);
	CARRY[i].push_back(0);

	i++;
	CARRY[i].push_back(b[2]);
	CARRY[i].push_back(0);
	CARRY[i].push_back(0);
	CARRY[i].push_back(1);
	Log::matrix(CARRY, "CARRY");

	bool optimal = false;
	int counter = 0;
	const int max_iterations = 1000;
	
	unsigned m = A.size();
	unsigned n = A[0].size();
	cout << "Dimension of the problem m x n: " << m << " x " << n << endl;
	
	while (optimal == false) {
		counter++;

		// Pricing
		unsigned s = 0;
		optimal = false;
		vector<cl_RA> costs;
		for (unsigned j = 0; j < n; j++) {
			bool isBasic = false;

			for (unsigned k = 0; k < basis.size(); k++) {
				cout << "Checking if column j = " << (j+basis.size()+1) << " already is a basic column. basis[" << k << "] = " << basis[k] << endl;
				if (basis[k] == (j+basis.size()+1)) {
					cout << "Column " << (j+basis.size()+1) << "is already in the basis" << endl;
					isBasic = true;
					break;
				}
			}
			if (isBasic == true) {
				costs.push_back(0);
				continue;
			}

			// pi^T A_j
			cl_RA cost = 0;
			for (unsigned k = 1; k < (m+1); k++) {
				cost += CARRY[0][k]*A[k-1][j];
				cout << "Multiply " << CARRY[0][k] << " * " << A[k-1][j] << endl;
			}
			cout << "Cost c = " << cost << endl;

			cl_RA d = 0;
			for (unsigned k = 0; k < m; k++) {
				d -= A[k][j];
			}
			cout << "d = " << d << endl;
			cost = d + cost;
			costs.push_back(cost);

			cout << "Reduced cost = " << cost << endl;
			if (cost < 0) {
				cout << "Reduced cost = " << cost << " < 0 at column j = " << j << endl;
				// s = j;
				// optimal = false;
				// break;
			}
		}
		Log::vec(costs, "costs");
		cl_RA min_cost = costs[0];
		// Find minimum cost
		for (unsigned i = 1; i < costs.size(); i++) {
			if (costs[i] < min_cost) {
				min_cost = costs[i];
				s = i;
			}
		}
		cout << "Minimum reduced cost = " << min_cost << " for index s = " << s << endl;
		if (min_cost >= 0) {
			cout << "All minimum reduced costs > 0 => we're optimal!" << endl;
			optimal = true;
			Log::matrix(CARRY, "CARRY");
			Log::vec(basis, "basis");
			break;
		}
		// We have found the column (index s) that should enter the basis

		// Column generation
		vector< cl_RA > Xs;
		Xs.clear();
		for (unsigned i = 1; i < (m+1); i++) { // loop through rows
			cl_RA row_sum = 0;
			for (unsigned j = 1; j < (m+1); j++) {
				cout << "Indices: " << i << "," << j << "," << s << endl;
				row_sum += CARRY[i][j]*A[j-1][s];
			}
			Xs.push_back(row_sum);
		}
		Log::vec(Xs, "Xs");

		// Determine pivot element
		cl_RA min = -1;
		unsigned r = 0;
		for (unsigned i = 1; i < (m+1); i++) {
			if (Xs[i-1] == 0) {
				cout << "Xs[" << (i-1) << "] == 0" << endl;
				continue;
			}
			cl_RA cur = CARRY[i][0]/Xs[i-1];
			cout << "Pivot here? " << cur << endl;
			if (cur < 0) {
				cout << "Current value negative: " << cur << endl;
				continue;
			}
			if (min < 0 || (min > 0 && cur < min)) {
				min = cur;
				r = i-1;
				cout << "r = " << r << " seems to be a suitable pivot element." << endl;
			}
		}
		if (min < 0) {
			cout << "ERROR: cost seems to be unbounded" << endl;
		}

		cout << "Pivot element determined: r,s = " << r << "," << s << endl;
		// Pivot

		// Make sure the target matrix B holds a copy of the inital matrix A
		vector< vector< cl_RA > > CARRY_Xs;
		CARRY_Xs = CARRY;
		CARRY_Xs[0].push_back(costs[s]);
		CARRY_Xs[1].push_back(Xs[0]);
		CARRY_Xs[2].push_back(Xs[1]);
		CARRY_Xs[3].push_back(Xs[2]);

		Log::matrix(CARRY_Xs, "CARRY_Xs");
		Matrix::pivot(CARRY_Xs, CARRY_Xs, r+1, CARRY_Xs[0].size()-1);
		Log::matrix(CARRY_Xs, "CARRY_Xs");

		basis[r] = CARRY[0].size()+s;
		Log::vec(basis, "basis");

		// Update carry matrix
		for (unsigned i = 0; i < CARRY.size(); i++) {
			for (unsigned j = 0; j < CARRY[i].size(); j++) {
				CARRY[i][j] = CARRY_Xs[i][j];
			}
		}

		if (counter > max_iterations) {
			optimal = true;
		}
	}
}

void Simplex::phase2()
{
}
