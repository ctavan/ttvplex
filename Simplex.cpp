#include "Simplex.h"

using namespace std;

// Constructor
Simplex::Simplex() {
	optimal = false;
	phase = 1;
}

void Simplex::init(LPParser& lp)
{
	ldbg << "Filling vector b with righthandsides from the constraints.\n";
	b.clear();
	for (unsigned i = 0; i < lp.constraints.size(); i++)
	{
		b.push_back(lp.constraints[i].rhs);
	}
	ldbg.vec(b, "b");

	ldbg << "Filling matrix A with coefficients from the constraints.\n";
	A.clear();
	for (unsigned i = 0; i < lp.constraints.size(); i++)
	{
		// Create a new row for each constraint
		A.push_back(vector<mpq_class>());
		// Loop through all variables, that are in the system
		for (unsigned j = 0; j < lp.variables.elements.size(); j++)
		{
			// For each variable, check if it is used in the current constraint
			mpq_class val = 0;
			for (unsigned k = 0; k < lp.constraints[i].elements.size(); k++)
			{
				if (lp.constraints[i].elements[k].name == lp.variables.elements[j])
				{
					val = lp.constraints[i].elements[k].coeff;
					break;
				}
				// ldbg << "Variable " << lp.variables.elements[j] << " is in constraint " << lp.constraints[i].name << "\n";
			}
			A[i].push_back(val);
		}
	}
	ldbg.matrix(A, "A");

	ldbg << "Filling vector c with costs from the objective.\n";
	c.clear();
	// Loop through all variables, that are in the system
	for (unsigned j = 0; j < lp.variables.elements.size(); j++)
	{
		mpq_class val = 0;
		for (unsigned i = 0; i < lp.objective.elements.size(); i++)
		{
			if (lp.variables.elements[j] == lp.objective.elements[i].name)
			{
				val = lp.objective.elements[i].coeff;
				break;
			}
		}
		c.push_back(val);
	}
	ldbg.vec(c, "c");

	ldbg << "Setting up CARRY-Matrix.\n";
	CARRY.clear();
	// First line of carry-matrix: reduced costs
	CARRY.push_back(vector<mpq_class>());
	CARRY[0].push_back(0);
	// Index set for basis columns
	basis.clear();
	for (unsigned i = 0; i < b.size(); i++)
	{
		CARRY[0][0] -= b[i];
		CARRY[0].push_back(0);
		basis.push_back(i+1);
	}
	for (unsigned i = 1; i <= b.size(); i++)
	{
		CARRY.push_back(vector<mpq_class>());
		CARRY[i].push_back(b[i-1]);
		for (unsigned j = 1; j <= b.size(); j++)
		{
			if (i == j)
			{
				CARRY[i].push_back(1);
			}
			else
			{
				CARRY[i].push_back(0);
			}
		}
	}
	ldbg.matrix(CARRY, "CARRY");

	// // Index set for basis columns
	// basis.clear();
	// basis.push_back(1);
	// basis.push_back(2);
	// basis.push_back(3);
	// 
	// // Righthand side
	// b.clear();
	// b.push_back(1);
	// b.push_back(3);
	// b.push_back(4);
	// ldbg.vec(b, "b");
	// 
	// // Matrix A
	// A.clear();
	// A.push_back(vector<mpq_class>());
	// A.push_back(vector<mpq_class>());
	// A.push_back(vector<mpq_class>());
	// A[0].push_back(3);
	// A[0].push_back(2);
	// A[0].push_back(1);
	// A[0].push_back(0);
	// A[0].push_back(0);
	// 
	// A[1].push_back(5);
	// A[1].push_back(1);
	// A[1].push_back(1);
	// A[1].push_back(1);
	// A[1].push_back(0);
	// 
	// A[2].push_back(2);
	// A[2].push_back(5);
	// A[2].push_back(1);
	// A[2].push_back(0);
	// A[2].push_back(1);
	// ldbg.matrix(A, "A");
	// 
	// // Cost coefficients c
	// c.clear();
	// c.push_back(1);
	// c.push_back(1);
	// c.push_back(1);
	// c.push_back(1);
	// c.push_back(1);
	// ldbg.vec(c, "c");
	// 
	// 
	// // // Carry Matrix
	// // CARRY.clear();
	// // CARRY.push_back(vector<mpq_class>());
	// // CARRY.push_back(vector<mpq_class>());
	// // CARRY.push_back(vector<mpq_class>());
	// // CARRY.push_back(vector<mpq_class>());
	// // CARRY.push_back(vector<mpq_class>());
	// // 
	// // int i = 0;
	// // CARRY[i].push_back(0);
	// // CARRY[i].push_back(0);
	// // CARRY[i].push_back(0);
	// // CARRY[i].push_back(0);
	// // CARRY[i].push_back(1);
	// // CARRY[i].push_back(1);
	// // CARRY[i].push_back(1);
	// // CARRY[i].push_back(1);
	// // CARRY[i].push_back(1);
	// // 
	// // i++;
	// // CARRY[i].push_back(0);
	// // CARRY[i].push_back(1);
	// // CARRY[i].push_back(1);
	// // CARRY[i].push_back(1);
	// // CARRY[i].push_back(0);
	// // CARRY[i].push_back(0);
	// // CARRY[i].push_back(0);
	// // CARRY[i].push_back(0);
	// // CARRY[i].push_back(0);
	// // 
	// // i++;
	// // CARRY[i].push_back(b[0]);
	// // CARRY[i].push_back(1);
	// // CARRY[i].push_back(0);
	// // CARRY[i].push_back(0);
	// // CARRY[i].push_back(3);
	// // CARRY[i].push_back(2);
	// // CARRY[i].push_back(1);
	// // CARRY[i].push_back(0);
	// // CARRY[i].push_back(0);
	// // 
	// // i++;
	// // CARRY[i].push_back(b[1]);
	// // CARRY[i].push_back(0);
	// // CARRY[i].push_back(1);
	// // CARRY[i].push_back(0);
	// // CARRY[i].push_back(5);
	// // CARRY[i].push_back(1);
	// // CARRY[i].push_back(1);
	// // CARRY[i].push_back(1);
	// // CARRY[i].push_back(0);
	// // 
	// // i++;
	// // CARRY[i].push_back(b[2]);
	// // CARRY[i].push_back(0);
	// // CARRY[i].push_back(0);
	// // CARRY[i].push_back(1);
	// // CARRY[i].push_back(2);
	// // CARRY[i].push_back(5);
	// // CARRY[i].push_back(1);
	// // CARRY[i].push_back(0);
	// // CARRY[i].push_back(1);
	// // ldbg.matrix(CARRY, "CARRY");
	// // 
	// // Matrix::row_subtract(CARRY, 1, 2);
	// // Matrix::row_subtract(CARRY, 1, 3);
	// // Matrix::row_subtract(CARRY, 1, 4);
	// // ldbg.matrix(CARRY, "CARRY");
	// // Matrix::pivot(CARRY, CARRY, 2, 4);
	// // ldbg.matrix(CARRY, "CARRY");
	// // Matrix::pivot(CARRY, CARRY, 2, 5);
	// // ldbg.matrix(CARRY, "CARRY");
	// // Matrix::pivot(CARRY, CARRY, 3, 7);
	// // ldbg.matrix(CARRY, "CARRY");
	// // Matrix::pivot(CARRY, CARRY, 4, 8);
	// // ldbg.matrix(CARRY, "CARRY");
	// // ldbg.message("=================");
	// 
	// // Carry-Matrix
	// CARRY.clear();
	// CARRY.push_back(vector<mpq_class>());
	// CARRY.push_back(vector<mpq_class>());
	// CARRY.push_back(vector<mpq_class>());
	// CARRY.push_back(vector<mpq_class>());
	// 
	// int i = 0;
	// CARRY[i].push_back(-b[0]-b[1]-b[2]);
	// CARRY[i].push_back(0);
	// CARRY[i].push_back(0);
	// CARRY[i].push_back(0);
	// 
	// i++;
	// CARRY[i].push_back(b[0]);
	// CARRY[i].push_back(1);
	// CARRY[i].push_back(0);
	// CARRY[i].push_back(0);
	// 
	// i++;
	// CARRY[i].push_back(b[1]);
	// CARRY[i].push_back(0);
	// CARRY[i].push_back(1);
	// CARRY[i].push_back(0);
	// 
	// i++;
	// CARRY[i].push_back(b[2]);
	// CARRY[i].push_back(0);
	// CARRY[i].push_back(0);
	// CARRY[i].push_back(1);
	// linf.matrix(CARRY, "CARRY");
	// exit(0);

	// Size of the problem. The real tableau would have size (m+1) x (n+1).
	// The carry-matrix has size (m+1) x (m+1)
	// The coefficient-matrix has size m x n
	// There are n cost-coefficients
	m = A.size();		// Rows of the problem
	n = A[0].size();	// Columns of the problem
	lout << "Dimension of the coefficient matrix A: " << m << " x " << n;
}

void Simplex::optimize()
{
	int counter = 0;					// Counter to avoid infinite loops
	const int max_iterations = 1000;	// Maximum number of iterations to be performed

	optimal = false;
	phase = 1;

	unsigned s;							// Index of the column that will enter the basis
	mpq_class cost_s;				// Reduced cost of the column that will enter the basis
	vector<mpq_class> X_s;			// Column X_s that is automatically generated by column generation in each step

	// Repeat simplex algorithm until an optimal solution is found
	while (optimal == false)
	{

		// In the beginning of each cycle we set optimal = false. If we get optimal in this cycle
		// this variable will be set to true by the pricing routine.
		optimal = false;

		// Pricing
		pricing(cost_s, s);	// Index of the column that will enter the basis wrt. the matrix A
		linf << "Pricing operation selected column: " << s << " with reduced cost " << cost_s << "\n";
		if (optimal == true)
		{
			// Phase 1: We reached optimality but objective > 0 => Infeasible. (@see Case 2 in Papadimitriou/Steiglitz, p. 56)
			if (phase == 1 && CARRY[0][0] > 0)
			{
				lout << "PHASE 1: All minimum reduced costs > 0 but objective xi = " << CARRY[0][0] << " > 0 => Infeasible!" << "\n";
			}
			// Phase 1: Objective is 0 -> Check if there are artificial variables left in the basis
			if (phase == 1 && CARRY[0][0] == 0)
			{
				for (unsigned i = 0; i < basis.size(); i++)
				{
					ldbg << "Checking if basis column b[" << i << "] = " << basis[i] << " is artificial variable, i.e. belongs to column 1 - " << m << " (0th col is RHS).\n";
					if (basis[i] <= m)
					{
						lout << "PHASE 1: Optimal solution found, but basis column b[" << i << "] = " << basis[i] << " is an artificial variable\n";
					}
				}
				lout << "PHASE 1: All minimum reduced costs > 0 => we're optimal!" << "\n";
			}
			linf.matrix(CARRY, "CARRY");
			linf.vec(basis, "basis");
			break;
		}

		// Generate column s (is written to vector X_s)
		generate_col(X_s, s, cost_s);

		// Determine pivot element
		mpq_class min = -1;
		unsigned r = 0;
		for (unsigned i = 1; i < (m+1); i++) {
			if (X_s[i] == 0) {
				ldbg << "X_s[" << (i) << "] == 0" << "\n";
				continue;
			}
			mpq_class cur = CARRY[i][0]/X_s[i];
			ldbg << "Pivot here? " << cur << "\n";
			if (cur < 0) {
				ldbg << "Current value negative: " << cur << "\n";
				continue;
			}
			if (min < 0 || (min > 0 && cur < min)) {
				min = cur;
				r = i-1;
				linf << "r = " << r << " seems to be a suitable pivot element." << "\n";
			}
		}
		if (min < 0) {
			lerr << "ERROR: cost seems to be unbounded" << "\n";
		}

		linf << "Pivot element determined: r,s = " << r << "," << s << "\n";
		// Pivot

		// Make sure the target matrix B holds a copy of the inital matrix A
		vector< vector<mpq_class> > CARRY_X_s;
		Matrix::append_vec(CARRY_X_s, CARRY, X_s);

		ldbg.matrix(CARRY_X_s, "CARRY_X_s");
		Matrix::pivot(CARRY_X_s, CARRY_X_s, r+1, CARRY_X_s[0].size()-1);
		ldbg.matrix(CARRY_X_s, "CARRY_X_s");

		basis[r] = CARRY[0].size()+s;
		linf.vec(basis, "basis");

		// Update carry matrix
		for (unsigned i = 0; i < CARRY.size(); i++) {
			for (unsigned j = 0; j < CARRY[i].size(); j++) {
				CARRY[i][j] = CARRY_X_s[i][j];
			}
		}

		if (counter > max_iterations) {
			lerr << "Still no result after max_iterations\n";
			break;
		}
		counter++;
	}
}

void Simplex::phase2()
{
}

void Simplex::pricing(mpq_class& cost_s, unsigned& s)
{
	s = 0;
	vector<mpq_class> costs;
	for (unsigned j = 0; j < n; j++) {
		bool is_basic = false;

		for (unsigned k = 0; k < basis.size(); k++) {
			ldbg << "Checking if column j = " << (j+basis.size()+1) << " already is a basic column. basis[" << k << "] = " << basis[k] << "\n";
			if (basis[k] == (j+basis.size()+1)) {
				ldbg << "Column " << (j+basis.size()+1) << "is already in the basis" << "\n";
				is_basic = true;
				break;
			}
		}
		if (is_basic == true) {
			costs.push_back(1); // Set positive fake cost to fill up the costs-vector
			continue;
		}

		// pi^T A_j
		mpq_class cost = 0;
		for (unsigned k = 1; k < (m+1); k++) {
			cost += CARRY[0][k]*A[k-1][j];
			ldbg << "Multiply " << CARRY[0][k] << " * " << A[k-1][j] << "\n";
		}
		ldbg << "Cost c = " << cost << "\n";

		mpq_class d = 0;
		if (phase == 1)
		{
			for (unsigned k = 0; k < m; k++) {
				d -= A[k][j];
			}
			ldbg << "d = " << d << "\n";
			cost = d + cost;
		}
		if (phase == 2)
		{
			d = c[j];
			ldbg << "d = c_" << j << " = " << d << "\n";
		}
		costs.push_back(cost);

		ldbg << "Phase " << phase << " reduced cost = " << cost << "\n";
	}

	ldbg.vec(costs, "costs");
	mpq_class min_cost = costs[0];
	// Find minimum cost
	for (unsigned i = 1; i < costs.size(); i++) {
		if (costs[i] < min_cost) {
			min_cost = costs[i];
			s = (int)i;
		}
	}

	ldbg << "Minimum reduced cost = " << min_cost << " for index s = " << s << "\n";
	if (min_cost >= 0) {
		optimal = true;
		return;
	}
	cost_s = costs[s];
	// We have found the column (index s) that should enter the basis
}

void Simplex::generate_col(vector<mpq_class>& X_s, const unsigned& s, const mpq_class& cost_s)
{
	// Column generation
	ldbg << "Starting column generation.\n";
	X_s.clear();
	X_s.push_back(cost_s); // First row holds the cost
	for (unsigned i = 1; i < (m+1); i++) { // loop through rows
		mpq_class row_sum = 0;
		for (unsigned j = 1; j < (m+1); j++) {
			ldbg << "Indices: " << i << "," << j << "," << s << "\n";
			row_sum += CARRY[i][j]*A[j-1][s];
		}
		X_s.push_back(row_sum);
	}
	linf << "Generated column:\n";
	linf.vec(X_s, "X_s");
}