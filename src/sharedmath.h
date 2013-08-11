/***************************************************************************
 *   Copyright (C) 2005-2006 by Ryan Lothian                               *
 *   Unauthorised reproduction of this program is prohibited               *
 *   ryan <at> ronline.no-ip.info                                          *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 ***************************************************************************/
 
#ifndef INC_sharedmath
#define INC_sharedmath

#include "shared.h" // for string manipulation etc.
#include "math.h"

const double MATHCONST_E 		 			= 2.71828182845904523536028747135;
const double MATHCONST_PI		 			= 3.14159265358979323846264338328;
const double GRADIENTRECURSIONTOLERANCE		= 0.994;
const int MAXDYNAMICSAMPLINGRECURSION		= 9;

struct LogicalCoordinate
{
	double dLogicalParameter, dLogicalResult;
};

struct EvaluationValue
{
	double value;
	bool syntaxerror;
};

// remove duplicate co-ordinates from an array
int quickSortPruneSortedDuplications( std::vector<LogicalCoordinate> *cloaArray );


/* quickSortPartition()
	Partition an array in-place, as per the partitioning stage of the quicksort algorithm
	Uses random median of 3 and pivot grouping ('fat pivots') optimisations
	TESTED: Unit test 6
*/
void quickSortPartition( std::vector<LogicalCoordinate> *cloaArray, int nStartElement, int nEndElement, int &nFirstArrayEnd, int &nSecondArrayStart );

/* quickSort()
	Partially quicksort an array between two elements (sorted so that each element is
    within a constant tolerance of its correct place)
	TESTED: Unit test 7
*/
void quickSort( std::vector<LogicalCoordinate> *cloaArray, int nStartElement, int nEndElement );

/* quickSortWrap()
	Fully quicksort an array of co-ordinates by logical parameter
	TESTED: Unit test 7
*/
void quickSortWrap( std::vector<LogicalCoordinate> *cloaArray );

/* factorial()
	Use the mathematical gamma function to calculate the factorial of a number, including non-integer factorials
	TESTED: Unit test 4
*/
inline double factorial( double dValue )
{
	return tgamma( dValue + 1 );
}

/* evaluateRawExpression()
	Evaluate the value of a given mathematical expression for a given variable value
	TESTED: Unit test 6
*/
EvaluationValue evaluateRawExpression( const char **szExpression, double dVariableValue );


#endif /* INC_sharedmath */
