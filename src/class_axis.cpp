/***************************************************************************
 *   Copyright (C) 2005-2006 by Ryan Lothian                               *
 *   Unauthorised reproduction of this program is prohibited               *
 *   plotgraph <at> ryanlothian.com                                        *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 ***************************************************************************/
 
#include "class_axis.h"
#include "class_equation.h"

double CAxis::logicalToFractional( double dInput )
{
	return ( dInput - this->dMinimum ) / ( this->dMaximum - this->dMinimum );
}

// reduce or increase an integer value so that it fits within a range
int fitIntToRange( int nValue, int nMin, int nMax )
{
	int nReturn = nValue;
	if ( nReturn < nMin ) nReturn = nMin;
	else if ( nReturn > nMax ) nReturn = nMax;
	
	return nReturn;
}

void CAxis::addAxisMarking( double dValue, const std::string &strLabel )
{
	 // put the new marking at the end of the markings array
	markings.push_back( std::pair< double, std::string > ( dValue, strLabel ) );
}

std::vector< std::pair<double, std::string> > CAxis::getAxisMarkings()
{
	std::vector< std::pair<double, std::string> > v_Markings = this->markings;
	
	if (this->dInterval != 0)
	{	
		// add the standard fixed-interval markings
		double dMinimumLogical = (this->dMinimum < this->dMaximum) ? this->dMinimum : this->dMaximum;
		double dMaximumLogical = (this->dMinimum < this->dMaximum) ? this->dMaximum : this->dMinimum;
		double dIntervalLogical = (this->dInterval < 0) ? -this->dInterval : this->dInterval;
		
		for (double dLogicalValue = dMinimumLogical; dLogicalValue <= dMaximumLogical; dLogicalValue += dIntervalLogical)
		{
			v_Markings.push_back( std::pair<double, std::string>( dLogicalValue, doubleToString( dLogicalValue ) ) );
		}
	}
			
	return v_Markings;
}

void CAxisPair_Polar::drawAxes( int nAxisColor, int nGridColor, int nTextColor )
{
	// get an array of markings on the R axis
	this->startEquation( nAxisColor );

	double dMinTheta = this->AxisTheta.getMinimum();
	double dMaxTheta = this->AxisTheta.getMaximum();	
	
	this->AxisTheta.setMinimum( 0 );
	this->AxisTheta.setMaximum( MATHCONST_PI * 2.0 );
	
	std::vector< std::pair<double, std::string> > v_RMarkings = this->AxisR.getAxisMarkings();
	for ( unsigned int nMarking = 0; nMarking < v_RMarkings.size(); nMarking++ )
	{
		double dAxisValue = v_RMarkings.at( nMarking ).first;
		std::string strAxisLabel = v_RMarkings.at( nMarking ).second;
		
		if ( dAxisValue > 0 )
		{
			CEquation RAxisLine( "r=" + doubleToFixedString( dAxisValue ), nAxisColor );
			
			RAxisLine.plot( this->im, *this, 100, true );
		}
	}
	
	this->AxisTheta.setMinimum( dMinTheta );
	this->AxisTheta.setMaximum( dMaxTheta );
	
	// get an array of markings on the theta axis
	std::vector< std::pair<double, std::string> > v_ThetaMarkings = this->AxisTheta.getAxisMarkings();
	for ( unsigned int nMarking = 0; nMarking < v_ThetaMarkings.size(); nMarking++ )
	{
		double dAxisValue = v_ThetaMarkings.at( nMarking ).first;
		std::string strAxisLabel = v_ThetaMarkings.at( nMarking ).second;
		
		if ( dAxisValue < 2.0 * MATHCONST_PI ) // if the value is less than 2pi
		{
			CEquation ThetaAxisLine( "t=" + doubleToFixedString( dAxisValue ), nAxisColor );
			ThetaAxisLine.plot( this->im, *this, 4, true );
		}
	}

	this->finishEquation();
}

void CAxisPair_Cartesian::drawAxes( int nAxisColor, int nGridColor, int nTextColor )
{
	 // draw the x-axis, taking 4 samples
	CEquation XAxisLine( "y=0", nAxisColor );
	XAxisLine.plot( this->im, *this, 4 );
	
	// draw the y-axis, taking 4 samples
	CEquation YAxisLine( "x=0", nAxisColor );
	YAxisLine.plot( this->im, *this, 4 );
	
	std::vector< std::pair<double, std::string> > v_YMarkings = AxisY.getAxisMarkings(); // fetch an array of markings on the y-axis
	std::vector< std::pair<double, std::string> > v_XMarkings = AxisX.getAxisMarkings(); // fetch an array of markings on the x-axis

	std::cout << v_YMarkings.size() << " y markings and " << v_XMarkings.size() << " x markings" << std::endl;
	
	int FONTSIZE = globalFONTSIZE;
	char* FONTPATH = globalFONTPATH; // path to the font we will use for axis labels
	
	this->plotter->startEquation(); // signal the beginning of plotting a set of lines (marks on the axes)
	this->plotter->setLineParameters( nAxisColor );
	
	for ( unsigned int nMarking = 0; nMarking < v_XMarkings.size(); nMarking++ )
	{
		// bounding rectangle allows positioning of the text so it is properly aligned
		int nBoundingRectangle[8];
		
		double dAxisValue = v_XMarkings.at( nMarking ).first;
		std::string strAxisLabel = v_XMarkings.at( nMarking ).second;
		
		char szLabel[ strAxisLabel.length() + 1 ];
		strcpy( szLabel, strAxisLabel.c_str() );
		
		std::cout << "Marking x=" << dAxisValue << std::endl;
		
		// obtain brect so that we can size the image
		std::pair<double, double> realcoordinate = this->convertCoordinateLogicalToReal( dAxisValue, 0.0 );
		
		this->plotter->drawLine( realcoordinate.first, realcoordinate.second, realcoordinate.first, realcoordinate.second + 3 );
		
		if ( realcoordinate.first > rectBound.x1 + 10 && realcoordinate.first < rectBound.x2 - 10 )
		{
			gdImageStringFT( NULL, &nBoundingRectangle[0], nTextColor, FONTPATH, FONTSIZE, 0.0, 0, 0, szLabel );
		
			int nTextBottom = fitIntToRange( int(realcoordinate.second) + 8 - nBoundingRectangle[5], 0, this->im->sy - 1 );
			int nTextLeft = fitIntToRange( int(realcoordinate.first) - (nBoundingRectangle[2] / 2), 0, this->im->sy - 1 );
			
			// render the text onto the image
			char *szError = gdImageStringFT(im, &nBoundingRectangle[0], nTextColor, FONTPATH, FONTSIZE, 0.0, nTextLeft, nTextBottom, szLabel);
			if ( szError )
			{
				std::cout << "[DEBUG] Text draw error: " << szError << std::endl;
			}
		}
	}
	
	for ( unsigned int nMarking = 0; nMarking < v_YMarkings.size(); nMarking++ )
	{
		int nBoundingRectangle[8]; // bounding rectangle allows positioning of the text so it is properly aligned
		
		double dAxisValue = v_YMarkings.at( nMarking ).first;
		std::string strAxisLabel = v_YMarkings.at( nMarking ).second;
		
		char szLabel[ strAxisLabel.length() + 1 ];
		strcpy( szLabel, strAxisLabel.c_str() );
				
		std::pair<double, double> realcoordinate = this->convertCoordinateLogicalToReal( 0.0, dAxisValue );
		
		this->plotter->drawLine( realcoordinate.first, realcoordinate.second, realcoordinate.first - 3, realcoordinate.second );
		
		if ( realcoordinate.second > rectBound.y1 + 10 && realcoordinate.second < rectBound.y2 - 10 )
		{
			// obtain bounding rectangle allowing us to position the text properly
			gdImageStringFT( NULL, &nBoundingRectangle[0], nTextColor, FONTPATH, FONTSIZE, 0.0, 0, 0, szLabel );
			
			// vertically centred with mark on axis, right-side of text aligned just to the left of the mark
			int nTextBottom = fitIntToRange( int(realcoordinate.second) - (nBoundingRectangle[5] / 2), 0, this->im->sy - 1 );
			int nTextLeft = fitIntToRange( int(realcoordinate.first) - nBoundingRectangle[2] - 6, 0, this->im->sy - 1 );

			// render the text onto the image
			char *szError = gdImageStringFT(im, &nBoundingRectangle[0], nTextColor, FONTPATH, FONTSIZE, 0.0, nTextLeft, nTextBottom, szLabel);
			if ( szError )
			{
				std::cout << "[DEBUG] Text draw error: " << szError << std::endl;
			}
		}
	}
	
	this->plotter->finishEquation(); // signal the end of drawing the marks on the axes
}
