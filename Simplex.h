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
	\param mpq_class Reference to a variable which is used to return the reduced cost of column that will enter the basis
	\return unsigned The column index (relative to the entire tableau) of the column that will enter the basis
	\sa
**/
		void pricing(mpq_class& cost_s, unsigned& s);
/** \brief Column generation
	
		Generates the pivot column from the original coefficient matrix A and the CARRY-matrix. s is the index
		of the column of the original matrix A that should be generated for the current step.
	
	\author Christoph Tavan TU Berlin
	\date 2011-01-11
	\param vector Column X_s to be generated
	\param unsigned Index of the column to be generated (index relative to matrix A, not to the whole tableau)
	\param mpq_class Reduced cost of the column to be genaretd, will be written to row 0
	\return void
	\sa
**/
		void generate_col(vector<mpq_class>& X_s, const unsigned& s, const mpq_class& cost_s);

	public:
		Simplex();

/** \brief Initialize the tableaus
	
		Initialize the tableaus for the revised simplex algorithm.
	
	\author Christoph Tavan TU Berlin
	\date 2010-12-25
	\return void
	\sa
**/
		void init(LPParser& lp);


		void optimize();
		void phase2();

		unsigned m; //!< Number of rows of the initial matrix
		unsigned n; //!< Number of columns of the initial matrix
		bool optimal;	//!< Whether we're optimal or not. Used to stop the simplex iterations
		vector< vector< mpq_class > > A; //!< Initial coefficient matrix
		vector< mpq_class > b; //!< Initial righthand side
		vector< mpq_class > c; //!< Initial cost coefficients
		vector< vector< mpq_class > > CARRY; //!< Carry^0-Matrix
		vector< unsigned > basis; //!< Index-set of current matrix columns
};

#endif
