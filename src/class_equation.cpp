/***************************************************************************
 *   Copyright (C) 2005-2006 by Ryan Lothian                               *
 *   Unauthorised reproduction of this program is prohibited               *
 *   plotgraph <at> ryanlothian.com                                          *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 ***************************************************************************/

#include "class_equation.h"

// Initialisation - setEquation should be called later if this is used
CEquation::CEquation()
{
	this->strEquation = "";
	this->nColor = 0;
	return;
}

// Initialisation - sets equation and line colour
CEquation::CEquation( const std::string &strEquationSet, int nColorSet )
{
	this->strEquation = strEquationSet;
	this->nColor = nColorSet;
	return;
}

std::string CEquation::getEquation()
{
	return strEquation;
}

// set the equation (mathematical formula) to a new value
void CEquation::setEquation( const std::string &strSet )
{
	this->strEquation = strSet;
}

// get the line colour
int CEquation::getColor()
{
	return nColor;
}

// set the line colour
void CEquation::setColor( int nColorSet )
{
	this->nColor = nColorSet;
}

// the equation is given in terms of which axis
int CEquation::getPrimaryAxis()
{
	std::string strLeftTwo = this->strEquation.substr( 0, 2 );
	if ( strLeftTwo == "x=" ) //x is given in terms of y
		return 1;
	else if ( strLeftTwo == "y=" ) //y is given in terms of x
		return 2;
	else if ( strLeftTwo == "r=" ) //angle is given in terms of radius
		return 3;
	else if ( (strLeftTwo == "o=") || (strLeftTwo == "t=") ) //radius is given in terms of theta
		return 4;
	else	
	{
		std::cout << "Warning: You must give your equations in the form \"x=...\", \"y=...\", \"r=...\" or \"t=...\" .\n";	
		return 0;
	}
}


// draw the line representing this equation on to an image using a given pair of axes
void CEquation::plot( gdImagePtr im, CAxisPair &axisPair, unsigned int nSamples, bool bNoStartFinishEquation )
{
	CProfiler functionprofile ( "CEquation::plot" );
	std::cout << "[DEBUG] CEquation::plot() - { " << this->strEquation << " } - " << nSamples << " samples\n";
	
	axisPair.applyBoundingBox(); // enforce boundaries on the area of the image to which we will be drawing
	
	int nVariableAxis = this->getPrimaryAxis();
	
	int nNoJoin = 1, nAxisId = 0;
	
	switch ( nVariableAxis )
	{
		case 1: nAxisId = 0; break; //x
		case 2: nAxisId = 1; break; //y
		case 3: nAxisId = 0; break; //r
		case 4: nAxisId = 1; break; //theta
	}
	
	double dMinLogicalControlled    = axisPair.getMinimumLogical( nAxisId );
	double dMaxLogicalControlled    = axisPair.getMaximumLogical( nAxisId );
	//double dMinLogicalVarying       = axisPair.getMinimumLogical( !nAxisId );
	//double dMaxLogicalVarying       = axisPair.getMaximumLogical( !nAxisId );
	
	//process co-ordinates to remove any not on screen
	//std::cout << "[DEBUG] Min controlled: " << dMinLogicalControlled << std::endl;;
	//std::cout << "[DEBUG] Max controlled: " << dMaxLogicalControlled << std::endl;;
	
	
	long lStartTime = clock();

	std::list< LogicalCoordinate > cloaFunctionSamples = this->evaluateBetween( dMinLogicalControlled, dMaxLogicalControlled, nSamples, 0, axisPair, nAxisId ); //calculate a list of co-ordinates on the line
	long lEndTime = clock();		
	std::cout << "[DEBUG] Took " << double( lEndTime - lStartTime ) * 1000 / CLOCKS_PER_SEC << " ms to evaluate function for plotting\n";
	
	LogicalCoordinate cloaSample, cloaPreviousSample;

	if ( !bNoStartFinishEquation ) axisPair.startEquation( this->nColor );
	
	//now we will actually draw our function by "joining the dots"
	lStartTime = clock();		
		
	for ( std::list< LogicalCoordinate >::iterator itPlot = cloaFunctionSamples.begin();
		  itPlot != cloaFunctionSamples.end(); itPlot++ )
	{
		//std::cout << "[DEBUG] CEquation::plot():element " << nElement << "\n";
		cloaPreviousSample = cloaSample;
		cloaSample = *itPlot;
		
		int nFPClassification = fpclassify( cloaSample.dLogicalResult );
		if ( nFPClassification == FP_NORMAL || nFPClassification == FP_ZERO )
		{
			if ( nNoJoin ) // do not join this point to the previous
			{
				nNoJoin = 0;
				
				if ( nAxisId )
					axisPair.unconnectedPoint( cloaSample.dLogicalParameter, cloaSample.dLogicalResult );
				else
					axisPair.unconnectedPoint( cloaSample.dLogicalResult, cloaSample.dLogicalParameter );
			}
			else
			{
				if ( nAxisId )
					axisPair.connectPoint( cloaSample.dLogicalParameter, cloaSample.dLogicalResult );
				else
					axisPair.connectPoint( cloaSample.dLogicalResult, cloaSample.dLogicalParameter );
			}
		}
		else
			nNoJoin = 1;
	}
	
	lEndTime = clock();
	
	std::cout << "[DEBUG] Took " << double( lEndTime - lStartTime ) * 1000 / CLOCKS_PER_SEC << " ms to join the dots\n";
		
	if ( !bNoStartFinishEquation )
	{
		lStartTime = lEndTime;
		axisPair.finishEquation();
		lEndTime = clock();
		std::cout << "[DEBUG] Took " << double( lEndTime - lStartTime ) * 1000 / CLOCKS_PER_SEC << " ms to finish joining\n";	
	}
}

// evaluate the value of the right-hand-side of the equation between two values of the dependent variable
int CEquation::subEvaluate( std::string &strExpression, std::list< LogicalCoordinate >& Samples, std::list< LogicalCoordinate >::iterator itFirstSample, int nSamples, int nRecursionNumber, CAxisPair &axisPair, int nAxisId )
{	
	int nTotalSamples = nSamples;
	CProfiler functionprofile ( "CEquation::subEvaluate" );
	
	// generate the subsamples
	const char *szChanged = NULL;

	std::list< LogicalCoordinate >::iterator itLastSample = itFirstSample;
	itLastSample++;
	
	double dMinimum = itFirstSample->dLogicalParameter;
	double dMaximum = itLastSample->dLogicalParameter;	
	
	//std::cout << "subEvaluate between " << dMinimum << " and " << dMaximum << "\n";
	
	double dInterval = ( dMaximum - dMinimum ) / ( nSamples + 1.0 ); //distance between each sample taken at this recursion level
	double dParameter = dMinimum, dEvaluationReturn = 0.0;
	
	for ( int nSampleNumber = 0; nSampleNumber < nSamples; nSampleNumber++ )
	{
		dParameter += dInterval;
		szChanged = strExpression.c_str(); //value of the pointer szChanged is altered by function
		
		// perform expression evaluation/sampling
		EvaluationValue Expression = evaluateRawExpression( &szChanged, dParameter );
		dEvaluationReturn = Expression.value;
		
		LogicalCoordinate coaResult = { dParameter, dEvaluationReturn };
		Samples.insert( itLastSample, coaResult ); // insert before maximum
	}
	
	rect rectBound = axisPair.getBoundingBox();
	
	if ( nRecursionNumber < MAXDYNAMICSAMPLINGRECURSION ) //prevents oversampling by limiting the maximum recursions
	{
		// now check whether more sampling should be done between these subsamples
		double dNewGrad = 0.0, dPrevGrad = 0.0, dGradDiff = 0.0;
		std::list< LogicalCoordinate >::iterator itNextSample = itFirstSample;
	
		std::pair< double, double > coordinate, coordinate2;
		
		coordinate2 = nAxisId ?
				axisPair.convertCoordinateLogicalToReal( itFirstSample->dLogicalParameter, itFirstSample->dLogicalResult ) :
				axisPair.convertCoordinateLogicalToReal( itFirstSample->dLogicalResult, itFirstSample->dLogicalParameter );
	
		double dXDelta = coordinate2.first - coordinate.first;
		double dYDelta = coordinate2.second - coordinate.second;
		
		dNewGrad = dYDelta / dXDelta;
		
		bool bCoordinateAOnscreen = false;		
		bool bCoordinateBOnscreen = (coordinate2.first > rectBound.x1) && (coordinate2.first <= rectBound.x2) &&
									(coordinate2.second > rectBound.y1) && (coordinate2.second <= rectBound.y2);;
		
		for ( int nSampleNumber = 0; nSampleNumber <= nSamples; nSampleNumber++ )
		{
	
			itFirstSample = itNextSample;
			itNextSample++;
			
			dPrevGrad = dNewGrad;
			
			coordinate = coordinate2;
			coordinate2 = nAxisId ?
				axisPair.convertCoordinateLogicalToReal( itNextSample->dLogicalParameter, itNextSample->dLogicalResult ) :
				axisPair.convertCoordinateLogicalToReal( itNextSample->dLogicalResult, itNextSample->dLogicalParameter );

			bCoordinateAOnscreen = bCoordinateBOnscreen;
			bCoordinateBOnscreen = (coordinate2.first > rectBound.x1) && (coordinate2.first <= rectBound.x2) &&
								   (coordinate2.second > rectBound.y1) && (coordinate2.second <= rectBound.y2);													
			
			if ( bCoordinateAOnscreen || bCoordinateBOnscreen )
			{
				dXDelta = coordinate2.first - coordinate.first;
				dYDelta = coordinate2.second - coordinate.second;
				double dDistanceApart = ( dXDelta * dXDelta ) + ( dYDelta * dYDelta );
	
				//std::cout << "[DEBUG] Distance: " << dDistanceApart << "\n";
				
				dNewGrad = dYDelta / dXDelta;
				
				// use the dot-product
				dGradDiff = ( 1 + ( dNewGrad * dPrevGrad ) ) / sqrt( ( 1 + ( dNewGrad * dNewGrad ) ) * ( 1 + ( dPrevGrad * dPrevGrad ) ) );
				//dGradDiff = fabs(dNewGrad - dPrevGrad) / (fabs(dNewGrad) + fabs(dPrevGrad) + 0.01);
				
				//std::cout << "N: " << dNewGrad << ", P:" << dPrevGrad << ", D: " << dGradDiff << std::endl;
	
				//fprintf(stdout,"GradDiff at %f = %f\n",vcoaSamples.at(nSampleNumber).dLogicalParameter,dGradDiff);
				bool bUnusual[2] = { 1, 1 };
				bUnusual[0] = fpclassify( itFirstSample->dLogicalResult ) != FP_ZERO &&
							  fpclassify( itFirstSample->dLogicalResult ) != FP_NORMAL;
				bUnusual[1] = fpclassify( itNextSample->dLogicalResult ) != FP_ZERO &&
							  fpclassify( itNextSample->dLogicalResult ) != FP_NORMAL;							  
				
				// resample if the gradient changes too quickly between samples or there is a singularity (infinity)
				//if ( (dDistanceApart > 8.0) || ( dDistanceApart > 1.0 && ( dGradDiff < GRADIENTRECURSIONTOLERANCE ) ) || ( nSumUnusual > 0 && nSumUnusual < 2 ))
				bool bResample = ( dDistanceApart > 0.5 && ( bUnusual[0] ^ bUnusual[1] ) );
				bResample |= ( dDistanceApart > 1.0 && ( dGradDiff < GRADIENTRECURSIONTOLERANCE ) );
				bResample |= ( dDistanceApart > 10.0 );
				
				if ( bResample )
				{				
					//fprintf(stdout,"Resampling between %f and %f\n",vcoaSamples.at(nSampleNumber-1).dLogicalParameter,vcoaSamples.at(nSampleNumber+1).dLogicalParameter);
					std::vector< LogicalCoordinate > vcoaSubsamples;
					 
					nTotalSamples += this->subEvaluate( strExpression, Samples, itFirstSample, 1, nRecursionNumber + 1, axisPair, nAxisId );
				}
			}
		}
	}
	
	//std::cout << "~subEvaluate\n";
	return nTotalSamples;
}

// evaluate the value of the right-hand-side of the equation between two values of the dependent variable
std::list< LogicalCoordinate > CEquation::evaluateBetween( double dMinimum, double dMaximum, int nSamples, int nRecursionNumber, CAxisPair &axisPair, int nAxisId )
{
	std::string strExpression = this->getExpression();
		
	CProfiler functionprofile ( "CEquation::evaluateBetween" );
	std::list< LogicalCoordinate > Samples;

	// sample minimum and maximum
	{
		const char *szChanged = strExpression.c_str(); //value of the pointer szChanged is altered by function
		EvaluationValue Expression = evaluateRawExpression( &szChanged, dMinimum );
		double dEvaluationReturn = Expression.value;
		LogicalCoordinate coaResult = { dMinimum, dEvaluationReturn };
		Samples.push_back( coaResult ); // insert before maximum
	}
	{
		const char *szChanged = strExpression.c_str(); //value of the pointer szChanged is altered by function
		EvaluationValue Expression = evaluateRawExpression( &szChanged, dMaximum );
		double dEvaluationReturn = Expression.value;
		LogicalCoordinate coaResult = { dMaximum, dEvaluationReturn };
		Samples.push_back( coaResult ); // insert before maximum
	}
	

	int nTotalSamples = subEvaluate( strExpression, Samples, Samples.begin(), nSamples, 0, axisPair, nAxisId );
	printf( "[DEBUG] Total samples: %d\n", nTotalSamples );
	
	return Samples;
}
