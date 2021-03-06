#ifndef Simplex_h
#define Simplex_h

#include "global.h"

#include "LPParser.h"

using namespace std;

/** \brief  Revised Simplex Algorithm
	
	Solves linear programs in standard form using the revised simplex algorithm.
	
© Copyright 2010 TU Berlin - Christoph Tavan. All Rights Reserved.

	\author Christoph Tavan TU Berlin
	\author $LastChangedBy$
	\date 2010-12-25
	\date $LastChangedDate$
	\version $Rev$	\sa
**/
class Simplex {

	private:
		int phase;			//!< Phase of the simplex algorithm we're currently in
/** \brief Pricing operation
	
		Performs the pricing operation of the revised simplex algorithm. The non-basic columns are checked whether
		they should enter the basis. The column with minimum negative reduced cost will enter the basis. If all
		reduced costs are > 0 we know that we are optimal and the member variable "optimal" is set to true so that
		the simplex algorithm terminates.
		As a parameter a variable holding the reduced costs of the selected basis-entering column must be passed.
	
	\author Christoph Tavan TU Berlin
	\date 2010-12-27
	\param my_rational Reference to a variable which is used to return the reduced cost of column that will enter the basis
	\return unsigned The column index (relative to the entire tableau) of the column that will enter the basis
	\sa
**/
		void pricing(my_rational& cost_s, unsigned& s);
/** \brief Calculates the reduced cost of column j
	
		This method takes into consideration whether we're in phase 1 or phase 2 and calculates the reduced cost
		coefficient for column j accordingly.
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-11
	\param unsigned Column-index
	\return The reduced cost of column j
	\sa
**/
		my_rational redcost(const unsigned& j);
/** \brief Column generation
	
		Generates the pivot column X_s from the original coefficient matrix A and the CARRY-matrix. s is the index
		of the column of the original matrix A that should be generated for the current step.
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-11
	\param unsigned Index of the column to be generated (index relative to matrix A, not to the whole tableau)
	\param my_rational Reduced cost of the column to be genaretd, will be written to row 0
	\return void
	\sa
**/
		void generate_col(const unsigned& s, const my_rational& cost_s);
/** \brief Checks if the column j is a basic column
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-11
	\param unsigned Column index
	\return true or false
	\sa
**/
		bool is_basic(const unsigned& j);
/** \brief Chooses a suitable pivot element
	
		Chooses a suitable pivot row and writes the result into the variable r.
		r is an index with respect to the complete tableau.
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-11
	\param unsigned Row-index of the chosen pivot-element
	\param vector The column-vector that has been selected for pivoting before
	\return void
	\sa
**/
		void choose_pivot(unsigned& r, const vector<my_rational>& col);
/** \brief Generates and prints the current full tableau (only if verbosity >= 3)
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-11
	\return void
	\sa
**/
		void full_tableau();
/** \brief Dump carry matrix and basis vector to file for later use
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-14
	\return void
	\sa
**/
		void write_to_file();
	

	public:
		Simplex(LPParser &l) : phase(1), lp(l), optimal(false), infeasible(false) {};

/** \brief Initialize the tableaus
	
		Initialize the tableaus for the revised simplex algorithm.
		If the files input_carry and input_basis exist, the CARRY-matrix and the basis
		are read from these files. This can be used to continue the simplex algorithm
		from a given state.
	
	\author Christoph Tavan TU Berlin
	\date 2010-12-25
	\param string Filename of a carry-matrix-dump
	\param string Filename of a basis-dump
	\return void
	\sa
**/
		void init(const string& input_carry, const string& input_basis, const int& start_phase = 1);
/** \brief The main simplex algorithm
	
		Optimizes the given LP using the revised simplex algorithm.
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-14
	\return void
	\sa
**/
		void optimize();
/** \brief Prints out the objective value
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-14
	\return void
	\sa
**/
		void objective();
/** \brief Prints out the nonzero variables and their values
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-14
	\return void
	\sa
**/
		void variables();

		LPParser& lp;	//!< Reference to the lp object
		unsigned m; //!< Number of rows of the initial matrix
		unsigned n; //!< Number of columns of the initial matrix
		bool optimal;	//!< Whether we're optimal or not. Used to stop the simplex iterations
		bool infeasible;	//!< Whether the problem is infeasible. Defaults to false
		vector< vector< my_rational > > A; //!< Initial coefficient matrix
		vector< my_rational > b; //!< Initial righthand side
		vector< my_rational > c; //!< Initial cost coefficients
		vector< vector< my_rational > > CARRY; //!< Carry^0-Matrix
		vector<unsigned> basis; //!< Index-set of current matrix columns
		vector<my_rational> X_s; 	//!< Column that is generated by column generation
};

#endif
