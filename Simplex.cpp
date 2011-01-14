#include "Simplex.h"

using namespace std;

// Constructor
// Simplex::Simplex() {
// 
// }

void Simplex::init(const string& input_carry, const string& input_basis, const int& start_phase)
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
		A.push_back(vector<my_rational>());
		// Loop through all variables, that are in the system
		for (unsigned j = 0; j < lp.variables.elements.size(); j++)
		{
			// For each variable, check if it is used in the current constraint
			my_rational val = 0;
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
		my_rational val = 0;
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

	ldbg << "Setting up CARRY-Matrix and basis.\n";
 	CARRY.clear();
	basis.clear();
	// No input files given -> create new carry-matrix and basis
	if (input_carry == "" && input_basis == "")
	{
		// First line of carry-matrix: reduced costs
		CARRY.push_back(vector<my_rational>());
		CARRY[0].push_back(0);
		for (unsigned i = 0; i < b.size(); i++)
		{
			CARRY[0][0] -= b[i];
			CARRY[0].push_back(0);
			basis.push_back(i+1);
		}
		for (unsigned i = 1; i <= b.size(); i++)
		{
			CARRY.push_back(vector<my_rational>());
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
	}
	else
	{
		string line;
		ifstream infile;
		// read carry
		infile.open(input_carry.c_str(), ifstream::in);
		unsigned cnt = 0;
		while(getline(infile, line))
		{
			CARRY.push_back(vector<my_rational>());
			istringstream iss(line);
			string word;
			while(iss >> word) {
				CARRY[cnt].push_back((my_rational)word);
			}
			cnt++;
		}
		infile.close();
		// read basis
		infile.open(input_basis.c_str(), ifstream::in);
		while(getline(infile, line))
		{
			int bb;
			istringstream iss(line);
			iss >> bb;
			basis.push_back(bb);
		}
		infile.close();
		ldbg.vec(basis, "basis");
	}
	ldbg.matrix(CARRY, "CARRY");

	// Set startphase
	phase = start_phase;

	// Size of the problem. The real tableau would have size (m+1) x (n+1).
	// The carry-matrix has size (m+1) x (m+1)
	// The coefficient-matrix has size m x n
	// There are n cost-coefficients
	m = A.size();		// Rows of the problem
	n = A[0].size();	// Columns of the problem
	lout << "\nDimension of the coefficient matrix A: " << m << " x " << n << "\n\n";
}

void Simplex::optimize()
{
	int counter = 0;					// Counter to avoid infinite loops

	optimal = false;

	unsigned s;							// Column-index of the column that will enter the basis
	my_rational cost_s;					// Reduced cost of the column that will enter the basis
	unsigned r;							// Row-index of the pivot-element

	// Repeat simplex algorithm until an optimal solution is found
	while (optimal == false)
	{
		ldbg << "=============== Iteration " << counter << " / " << max_iterations << " ===============\n";

		if (counter%100 == 0) {
			lout << "PHASE " << phase << ": iteration " << counter << "\n";
		}

		// In the beginning of each cycle we set optimal = false. If we get optimal in this cycle
		// this variable will be set to true by the pricing routine.
		optimal = false;

		full_tableau();

		// Pricing
		pricing(cost_s, s);	// Index of the column that will enter the basis wrt. the matrix A
		linf << "Pricing operation selected column s = " << s << " with reduced cost " << cost_s << " for pivoting.\n";
		if (optimal == true)
		{
			// Phase 1: We reached optimality but objective > 0 => Infeasible. (@see Case 2 in Papadimitriou/Steiglitz, p. 56)
			if (phase == 1 && CARRY[0][0] < 0)
			{
				lout << "PHASE 1: All minimum reduced costs > 0 but objective xi = " << CARRY[0][0] << " > 0 => Infeasible! (after " << counter << " iterations)" << "\n";
				infeasible = true;
			}
			// Phase 1: Objective is 0 -> Check if there are artificial variables left in the basis
			if (phase == 1 && CARRY[0][0] == 0)
			{
				while (phase == 1)
				{
					for (unsigned i = 0; i < basis.size(); i++)
					{
						ldbg << "Checking if basis column b[" << i << "] = " << basis[i] << " is artificial variable, i.e. belongs to column 1 - " << m << " (0th col is RHS).\n";
						if (basis[i] > m)
						{
							if (i == basis.size()-1)
							{
								// At this point we can enter phase 2 (no artificial variables in the basis)
								phase = 2;
								write_to_file();
							}
							continue;
						}
						lout << "PHASE 1: Basic feasible solution found, but basis column basis[" << i << "] = " << basis[i] << " is an artificial variable\n";

						write_to_file();

						bool allzero = true;

						// Search for a nonzero, nonbasic column in the i'th row
						for (unsigned j = 0; j < n; j++)
						{
							linf << "~~ Checking column " << j << "\n";
							// Check if column j is a basic column
							if (is_basic(j))
							{
								continue;
							}
							linf << "~~ Generating non-basic column " << j << "\n";
							// Generate j'th column
							my_rational cost = redcost(j);
							generate_col(CARRY[0].size()+j, cost);
							ldbg << "x[i][j] = " << X_s[i+1] << "\n";
							// If ij-element is zero, we cant pivot, continue
							if (X_s[i+1] == 0)
							{
								continue;
							}
							// ij-element was not zero, so we can pivot here.
							allzero = false;
							linf << "Pivot with this element: r,s = " << i+1 << "," << CARRY[0].size()+j << " to get rid of the artificial variable basis[" << i << "] = " << basis[i] << " in the basis\n";

							vector< vector<my_rational> > CARRY_X_s;		// Make a copy of the carry matrix
							Matrix::append_vec(CARRY_X_s, CARRY, X_s);	// and append the newly generated column

							// Perform Pivot
							// full_tableau();
							// Pivot element in the carry-matrix is at row r (since 0th first row holds the cost
							// ) and either in one of the columns contained in CARRY or in the last column which is X_s
							Matrix::pivot(CARRY_X_s, CARRY_X_s, i+1, CARRY[0].size());

							// Update the basis: Basis values count with respect to the whole tableau!
							basis[i] = CARRY[0].size()+j;
							// lout.vec(basis, "basis");

							// Update carry matrix
							for (unsigned ii = 0; ii < CARRY.size(); ii++) {
								for (unsigned jj = 0; jj < CARRY[ii].size(); jj++) {
									CARRY[ii][jj] = CARRY_X_s[ii][jj];
								}
							}
							lout << "Monitoring xi: " << CARRY[0][0] << "\n";
							// full_tableau();
							// exit(EXIT_FAILURE);
						} // for
						// If not all entries in this row were zero, we recheck the basis
						if (!allzero) {
							i = 0;
							continue;
						}
						// All entries in this row were zero, so we can reduce the system size
						lout << "PHASE 1: All x_ij = 0 in row i = " << i+1 << "\n";
						ldbg << "erasing row " << i+1 << "\n";

						ldbg.vec(basis, "basis");
						basis.erase(basis.begin()+i);
						// Adjust basis-indices (subtract -1) since we remove a column from the tableau
						for (unsigned k = 0; k < basis.size(); k++)
						{
							if (basis[k] > i)
							{
								basis[k]--;
							}
						}
						ldbg.vec(basis, "basis");

						ldbg.vec(b, "b");
						b.erase(b.begin()+i);
						ldbg.vec(b, "b");

						ldbg.matrix(A, "A");
						A.erase(A.begin()+i);
						ldbg.matrix(A, "A");

						ldbg.matrix(CARRY, "CARRY");
						CARRY.erase(CARRY.begin()+i+1);
						m--;
						for (unsigned k = 0; k < CARRY.size(); k++)
						{
							CARRY[k].erase(CARRY[k].begin()+i+1);
						}
						ldbg.matrix(CARRY, "CARRY");
						full_tableau();

						lout << "PHASE 1: New Dimension of the coefficient matrix A: " << m << " x " << n << "\n";

						ldbg << "Removed row " << i+1 << " from the system!\n";
						i = 0;
					}
				} // while phase == 1
				lout << "PHASE 1: Found basic feasible solution after " << counter << " iterations => continuing with PHASE 2!\n";

				phase = 2;
				optimal = false;
				lout << "\n===================================================\n";
				lout << "===================================================\n\n";
				lout << "PHASE 2: Starting\n\n";
				// Remove artificial variables
				

				// Calculate new (real) reduced cost
				// loop over rows
				ldbg.matrix(CARRY, "CARRY");
				for (unsigned j = 0; j < basis.size(); j++)
				{
					// c[basis[i]-CARRY[0].size()]
					ldbg << "Calculating new reduced cost for column " << j << "\n";
					my_rational cost = 0;
					ldbg << "Summing: ";
					for (unsigned i = 0; i < basis.size(); i++)
					{
						cost -= c[basis[i]-CARRY[0].size()]*CARRY[i+1][j+1];
						ldbg << "+ " << c[basis[i]-CARRY[0].size()] << " * " << CARRY[i+1][j+1] << " ";
					}
					ldbg << "\nCost is: " << cost << "\n";
					CARRY[0][j+1] = cost;
				}
				// Calculate new objective value
				my_rational cost = 0;
				ldbg << "Calculating new objective value:\n";
				ldbg << "Summing: ";
				for (unsigned i = 0; i < basis.size(); i++)
				{
					cost -= c[basis[i]-CARRY[0].size()]*CARRY[i+1][0];
					ldbg << "+ " << c[basis[i]-CARRY[0].size()] << " * " << CARRY[i+1][0] << " ";
				}
				ldbg << "\nCost is: " << cost << "\n";
				CARRY[0][0] = cost;
				ldbg.matrix(CARRY, "CARRY");
				continue;
			} // phase == 1 && CARRY[0][0] == 0
			if (phase == 2 && optimal)
			{
				lout << "PHASE 2: Found optimal solution after " << counter << " iterations!\n";
			}
			ldbg.matrix(CARRY, "CARRY");
			ldbg.matrix(CARRY, "CARRY", true);
			ldbg.vec(basis, "basis");
			if (infeasible || optimal)
			{
				break;
			}
		}

		// If selected column is NOT inside CARRY, generate it!
		if (s >= CARRY[0].size())
		{
			// Generate column s (is written to member vector X_s)
			generate_col(s, cost_s);
		}
		else
		{
			// Extract column i from CARRY
			X_s.clear();
			for (unsigned i = 0; i < CARRY.size(); i++)
			{
				X_s.push_back(CARRY[i][s]);
			}
		}

		// Choose pivot element, i.e. determine row r. Pivot element is then x_rs
		choose_pivot(r, X_s);

		ldbg << "=====> Let column basis[" << r-1 << "] = " << basis[r-1] << " leave the basis and column " << s << " enter.\n";
		string enter;
		string leave;
		if (s < CARRY[0].size()) {
			stringstream out;
			out << s;
			enter = "artificial_" + out.str();
		} else {
			enter = lp.variables.elements[s-CARRY[0].size()];
		}
		if (basis[r-1] < CARRY[0].size()) {
			stringstream out;
			out << basis[r-1];
			leave = "artificial_" + out.str();
		} else {
			leave = lp.variables.elements[basis[r-1]-CARRY[0].size()];
		}
		linf << "=====> Let '" << enter << "' enter the basis and '" << leave << "' leave.\n";
		linf << "=====> Pivot element: r,s = " << r << "," << s << " (" << (r+1) << "," << (s+1) << ")\n\n";

		vector< vector<my_rational> > CARRY_X_s;		// Make a copy of the carry matrix
		Matrix::append_vec(CARRY_X_s, CARRY, X_s);	// and append the newly generated column

		// Perform Pivot
		ldbg.matrix(CARRY_X_s, "CARRY_X_s");
		// Pivot element in the carry-matrix is at row r (since 0th first row holds the cost
		// ) and either in one of the columns contained in CARRY or in the last column which is X_s
		Matrix::pivot(CARRY_X_s, CARRY_X_s, r, CARRY[0].size());
		ldbg.matrix(CARRY_X_s, "CARRY_X_s");

		// Update the basis: Basis values count with respect to the whole tableau!
		basis[r-1] = s;
		ldbg.vec(basis, "basis");

		// Update carry matrix
		for (unsigned i = 0; i < CARRY.size(); i++) {
			for (unsigned j = 0; j < CARRY[i].size(); j++) {
				CARRY[i][j] = CARRY_X_s[i][j];
			}
		}

		if (max_iterations > 0 && counter > max_iterations) {
			lerr << "Still no result after max_iterations\n";
			exit(EXIT_FAILURE);
		}
		counter++;
	}
}

void Simplex::pricing(my_rational& cost_s, unsigned& s)
{
	s = 0;

	// In Phase I First of all check through the reduced costs, that are present in the CARRY-matrix!
	if (phase == 1)
	{
		for (unsigned j = 1; j < CARRY[0].size(); j++)
		{
			ldbg << "Cost in CARRY: CARRY[0][" << j << "] = " << CARRY[0][j] << "\n";
			if (CARRY[0][j] < 0)
			{
				cost_s = CARRY[0][j];
				s = j;
				return;
			}
		}
	}

	vector<my_rational> costs;
	for (unsigned j = 0; j < n; j++) {
		if (is_basic(j)) {
			costs.push_back(1); // Set positive fake cost to fill up the costs-vector
			continue;
		}

		my_rational cost = redcost(j);
		costs.push_back(cost);

		ldbg << "Phase " << phase << " reduced cost = " << cost << " at column s = " << CARRY[0].size()+j << " (j = " << j << ")\n";

		// If the first negative cost that is found, just use it because we want
		// to prevent cycling using Blands rule.
		// @see e.g. Papadimitriou/Steiglitz p. 53
		if (cost < 0)
		{
			s = CARRY[0].size()+j;
			cost_s = cost;
			return;
		}
	}
	// We reach this point only if none of the costs was < 0. In that case we have found an optimal solution.
	optimal = true;
	return;

	// This following code would be used for choosing the column with minimum cost which would
	// be the most profitable way to choose a column.
	// However this approach doesn't prevent cycling, and following blands rule we selected just the *first*
	// negative reduced cost, not the smallest... The code remains here for reference.
	ldbg.vec(costs, "costs");
	my_rational min_cost = costs[0];
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

my_rational Simplex::redcost(const unsigned& j)
{
	// pi^T A_j
	my_rational cost = 0;
	for (unsigned k = 1; k < (m+1); k++) {
		cost += CARRY[0][k]*A[k-1][j];
		// ldbg << "Multiply " << CARRY[0][k] << " * " << A[k-1][j] << "\n";
	}
	ldbg << "Cost c = " << cost << "\n";

	my_rational d = 0;
	if (phase == 1)
	{
		for (unsigned k = 0; k < m; k++) {
			d -= A[k][j];
		}
		ldbg << "d = " << d << "\n";
	}
	if (phase == 2)
	{
		d = c[j];
		ldbg << "d = c_" << j << " = " << d << "\n";
	}
	cost = d + cost;
	return cost;
}

bool Simplex::is_basic(const unsigned& j)
{
	ldbg << "Checking if column j = " << (j+basis.size()+1) << " already is a basic column?";
	for (unsigned k = 0; k < basis.size(); k++) {
		// ldbg << "basis[" << k << "] = " << basis[k] << "\n";
		if (basis[k] == (j+basis.size()+1)) {
			ldbg << " => it is!\n";
			return true;
		}
	}
	ldbg << " => it is not!\n";
	return false;
}

void Simplex::generate_col(const unsigned& s, const my_rational& cost_s)
{
	// Column generation
	ldbg << "Starting column generation.\n";
	X_s.clear();
	X_s.push_back(cost_s); // First row holds the cost
	for (unsigned i = 1; i < (m+1); i++) { // loop through rows
		my_rational row_sum = 0;
		for (unsigned j = 1; j < (m+1); j++) {
			// ldbg << "Indices: " << i << "," << j << "," << s << "\n";
			row_sum += CARRY[i][j]*A[j-1][s-CARRY[0].size()];
		}
		X_s.push_back(row_sum);
	}
	ldbg << "Generated column:\n";
	ldbg.vec(X_s, "X_s");
}

void Simplex::choose_pivot(unsigned& r, const vector<my_rational>& col)
{
	// Determine pivot element according to blands anti-cycling rule.
	// @see e.g. Papadimitriou/Steiglitz p. 53

	vector<my_rational> thetas;		// Theta_i: step length of row i
	thetas.clear();

	my_rational theta_min = -1;		// Theta_0: Primal step length = min { Theta_i > 0 }

	// Loop through all rows
	for (unsigned i = 1; i < (m+1); i++)
	{
		// assure x_ij > 0  (col[i] = x_ij)
		if (col[i] <= 0)
		{
			ldbg << "col[" << (i) << "] = " << col[i] << " <= 0" << "\n";
			thetas.push_back(-1);
			continue;
		}
		// x_i0 / x_ij
		my_rational cur = CARRY[i][0]/col[i];
		thetas.push_back(cur);
		ldbg << "Pivot here? " << cur << "\n";
		if (theta_min < 0 || (theta_min > 0 && cur < theta_min))
		{
			theta_min = cur;
		}
	}
	// Smallest theta should be positive
	if (theta_min < 0)
	{
		lout << "WARNING: objective seems to be unbounded. Stopping here!" << "\n";
		exit(EXIT_SUCCESS);
	}
	ldbg << "Thetas for pivot element selection:\n";
	ldbg.vec(thetas, "thetas");
	// Now select the pivot element according to Blands rule
	// In case only one theta has attained the minimum value theta_min, we choose this row
	// In case of tie, choose the lowest numbered column basis[i] to leave the basis
	int temp = -1;	// Temporary variable to decide, which row corresponds to the column with smalles index
	int rtemp = -1;	
	for (unsigned i = 0; i < thetas.size(); i++)
	{
		// Only consider thetas with the same minimum value
		if (thetas[i] != theta_min)
		{
			continue;
		}
		ldbg << "Minimum theta[" << i << "] = " << theta_min << " found! Corresponds to basic column basis[" << i << "] = " << basis[i] << ". Current value of r = " << rtemp << "\n";
		// (basis[i]-CARRY[0].size()) is the column index of the column corresponding to the i'th basic variable
		// with respect to the original coefficient matrix A
		if (temp < 0 || (temp >= 0 && basis[i] < (unsigned)temp))
		{
			temp = basis[i];
			rtemp = i;
			ldbg << "Chose r = " << rtemp << " according to blands rule (temp = " << temp << ").\n";
		}
	}
	if (rtemp < 0)
	{
		lerr << "ERROR: Unable to find a suitable pivoting-row according to blands rule.\n";
		exit(EXIT_FAILURE);
	}
	r = (unsigned)rtemp;
	r++;
	linf << "Bland's rule selected row r = " << r << " for pivoting.\n";
}

void Simplex::objective()
{
	if (infeasible)
	{
		lout << "Problem infeasible!\n";
		return;
	}
	my_rational obj = 0;
	for (unsigned i = 0; i < basis.size(); i++)
	{
		// obj = x_basic * cost_basic
		obj += CARRY[i+1][0]*c[basis[i]-CARRY[0].size()];
	}
	if (lp.objective.direction == LPObjective::OBJ_MAX) {
		obj *= -1;
	}
	lout << "Objective: " << obj << " = " << (my_float)obj << "\n";
}
void Simplex::variables()
{
	if (infeasible)
	{
		lout << "Problem infeasible!\n";
		return;
	}
	ldbg.vec(basis, "basis");
	lout << "Variable name\t\t" << "Solution value\n";
	for (unsigned i = 0; i < basis.size(); i++)
	{
		// Don't show 0-variables
		if (CARRY[i+1][0] == 0)
		{
			continue;
		}
		lout << lp.variables.elements[basis[i]-CARRY[0].size()] << "\t\t\t" << CARRY[i+1][0] << " (" << (my_float)CARRY[i+1][0] << ")\n";
	}
	lout << "All other variables are 0.\n";
}

void Simplex::full_tableau()
{
	// Skip if not in debugging mode
	if (ldbg.level == 0) {
		return;
	}
	// Calculate the whole tableau (just for debugging)
	vector< vector<my_rational> > TABLEAU;		// Generate the whole tableau
	TABLEAU.clear();
	TABLEAU = CARRY;
	for (unsigned j = 0; j < n; j++)
	{
		my_rational cost = redcost(j);
		generate_col(CARRY[0].size()+j, cost);
		Matrix::append_vec(TABLEAU, TABLEAU, X_s);
	}
	ldbg.matrix(TABLEAU, "TABLEAU");
}

void Simplex::write_to_file()
{
	// Dump carry matrix and basis vector to file for later use
	ofstream cfile;
	cfile.open("carry.dat");
	for (unsigned e = 0; e < CARRY.size(); e++) {
		for (unsigned f = 0; f < CARRY[e].size(); f++) {
			cfile << (my_rational)CARRY[e][f] << "\t";
		}
		cfile << endl;
	}
	cfile.close();
	cfile.open("basis.dat");
	for (unsigned e = 0; e < basis.size(); e++) {
		cfile << basis[e] << endl;
	}
	cfile.close();
}