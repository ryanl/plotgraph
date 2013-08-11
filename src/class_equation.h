/***************************************************************************
 *   Copyright (C) 2005-2006 by Ryan Lothian                               *
 *   Unauthorised reproduction of this program is prohibited               *
 *   ryan <at> ronline.no-ip.info                                          *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 ***************************************************************************/

#ifndef INC_class_equation
#define INC_class_equation

#include <cstring>
#include <list>

#include "class_axis.h"

/* CEquation
	Class used to represent and perform actions based upon a mathematical equation
	TESTED: Module test 2
*/

class CEquation
{
	private:
		std::string strEquation;
		int	nColor;

		int	getPrimaryAxis();
		
		inline	std::string	getExpression()
		{
			return this->strEquation.substr( 2, this->strEquation.length() - 2 ) + ")      ";
		}
		
	public:
				CEquation();
				CEquation( const std::string &strEquationSet, int nColorSet );
		
		std::string	getEquation();
		std::list< LogicalCoordinate > evaluateBetween( double dMinimum, double dMaximum, int nSamples, int nRecursionNumber, CAxisPair &axisPair, int nAxisId);
		int subEvaluate( std::string &strExpression, std::list< LogicalCoordinate >& Samples, std::list< LogicalCoordinate >::iterator itFirstSample, int nSamples, int nRecursionNumber, CAxisPair &axisPair, int nAxisId );
		// returns true if the right-hand-side of the expression is valid
		inline bool checkSyntax()
		{
			const char *szExpression = this->getExpression().c_str();
			EvaluationValue Expression = evaluateRawExpression( &szExpression, 0.0 );
			return !Expression.syntaxerror;
		}
		
		// change the equation string
		void	setEquation( const std::string &strSet );
		
		int		getColor();
		void	setColor( int nColorSet );
		
		// plot the equation onto an image with a given co-ordinate system (axisPair), taking nSamples samples of the curve
		void	plot( gdImagePtr im, CAxisPair &axisPair, unsigned int nSamples, bool bNoStartFinishEquation = false  );
};

#endif
