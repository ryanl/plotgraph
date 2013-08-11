/***************************************************************************
 *   Copyright (C) 2005-2006 by Ryan Lothian                               *
 *   Unauthorised reproduction of this program is prohibited               *
 *   plotgraph <at> ryanlothian.com                                          *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 ***************************************************************************/
#include "class_linedrawer.h"

void CLineDrawer_Supersampled::startEquation()
{
	CProfiler functionprofile ( "CLineDrawer_Supersampled::startEquation" );
	unsigned int nTotalPixels = im->sx * im->sy;
	for (unsigned int nPixel = 0; nPixel < nTotalPixels; nPixel++)
	{
		this->nSuperPixels[nPixel] = 0; //erase pixel array
	}
}

void CLineDrawer_Supersampled::drawLine( double dX1, double dY1, double dX2, double dY2 )
{
	//CProfiler functionprofile ( "CLineDrawer_Supersampled::drawLine" );
			
	// round co-ordinate parts to the nearest supersample pixel
	dX1 *= nMultiplier;
	dX2 *= nMultiplier;
	dY1 *= nMultiplier;
	dY2 *= nMultiplier;
	
	/* try and prevent drawing off screen */				
	double dMinX = -4, dMinY = -4;
	double dMaxX = ( im->sx << 2 ) + 4, dMaxY = ( im->sy << 2 ) + 4;
	
	bool bOnscreen1 = ( dX1 >= dMinX && dX1 <= dMaxX && dY1 >= dMinY && dY1 <= dMaxY );
	bool bOnscreen2 = ( dX2 >= dMinX && dX2 <= dMaxX && dY2 >= dMinY && dY2 <= dMaxY );
	
	if ( !bOnscreen1 && !bOnscreen2 ) return;
	
	if ( dX2 > dMaxX )
	{
		dY2 = dY1 + ( (dY2 - dY1) / (dX2 - dX1) ) * (dMaxX - dX1);
		dX2 = dMaxX;
	}
	else if ( dX2 < dMinX )
	{
		dY2 = dY1 + ( (dY2 - dY1) / (dX2 - dX1) ) * (dMinX - dX1);
		dX2 = dMinX;
	}
	
	if ( dY2 > dMaxY )
	{
		dX2 = dX1 + ( (dX2 - dX1) / (dY2 - dY1) ) * (dMaxY - dY1);
		dY2 = dMaxY;
	}
	else if ( dY2 < dMinY )
	{
		dX2 = dX1 + ( (dX2 - dX1) / (dY2 - dY1) ) * (dMinY - dY1);
		dY2 = dMinY;
	}
	
	if ( dX1 > dMaxX )
	{
		dY1 = dY2 + ( (dY1 - dY2) / (dX1 - dX2) ) * (dMaxX - dX2);
		dX1 = dMaxX;
	}
	else if ( dX1 < dMinX )
	{
		dY1 = dY2 + ( (dY1 - dY2) / (dX1 - dX2) ) * (dMinX - dX2);
		dX1 = dMinX;
	}
	
	if ( dY1 > dMaxY )
	{
		dX1 = dX2 + ( (dX1 - dX2) / (dY1 - dY2) ) * (dMaxY - dY2);
		dY1 = dMaxY;
	}
	else if ( dY1 < dMinY )
	{
		dX1 = dX2 + ( (dX1 - dX2) / (dY1 - dY2) ) * (dMinY - dY2);
		dY1 = dMinY;
	}
	
	int nX1 = intround( dX1 ), nY1 = intround( dY1 ), nX2 = intround( dX2 ), nY2 = intround( dY2 );
	
	int nSteep = abs( nY2 - nY1 ) > abs( nX2 - nX1 );
	if ( nSteep ) // if the absolute value of the gradient of the line is > 1
	{
		swapv( nX1, nY1 );
		swapv( nX2, nY2 );
	}
	if ( nX1 > nX2 )
	{
		swapv( nX1, nX2 );
		swapv( nY1, nY2 );
	}
	int nXDelta = nX2 - nX1;
	int nYDelta = abs( nY2 - nY1 );
	int nError = 0;
	int nDeltaError = nYDelta;
	
	int nY = nY1;
	int nYStep = ( nY1 < nY2 ) ? 1 : -1;
	
	 // plot an rough circle centered at each pixel on the line - map of pixels to set
	int nPlotMap[5][5] = {
		{ 0, 0, 1, 0, 0 },
		{ 0, 1, 1, 1, 0 },
		{ 1, 1, 1, 1, 1 },
		{ 0, 1, 1, 1, 0 },
		{ 0, 0, 1, 0, 0 }
	};
	
	for ( int nX = nX1; nX <= nX2; nX++ ) // from the start pixel to the end pixel
	{
		if ( nSteep )
		{
			for ( int nXOffset = -2; nXOffset <= 2; nXOffset++ )
			{
				for ( int nYOffset = -2; nYOffset <= 2; nYOffset++ )
				{
					if ( nPlotMap[nXOffset + 2][nYOffset + 2] )
						this->plotPixel( nY + nYOffset, nX + nXOffset ); // plot each pixel in the brush
				}
			}
		}
		else
		{
			for ( int nXOffset = -2; nXOffset <= 2; nXOffset++ )
			{
				for ( int nYOffset = -2; nYOffset <= 2; nYOffset++ )
				{
					if ( nPlotMap[nXOffset + 2][nYOffset + 2] )
						this->plotPixel( nX + nYOffset, nY + nXOffset );
				}
			}
		}
		nError += nDeltaError;
		if ( ( nError << 1 ) >= nXDelta ) // if it is time for y to increase
		{
			nY += nYStep;
			nError -= nXDelta;
		}
	}
}

void CLineDrawer_Supersampled::finishEquation()
{
	long lStartTime = clock();
	
	CProfiler functionprofile ( "CLineDrawer_Supersampled::finishEquation" );
	unsigned int nPixel = 0;
	
	// down-sample every pixel in the entire entire image
	for ( int nX = 0; nX < im->sx; ++nX ) 
	{
		for ( int nY = 0; nY < im->sy; ++nY )
		{
			unsigned int nPixelOpacity = nBitsSetLookupTable[ this->nSuperPixels[nPixel] ];
			
			++nPixel; // go to next pixel in the array
			
			if ( nPixelOpacity )
			{
				// calculate the opacity of the pixel that will drawn to the original bitmap image
				int nDrawColor = gdTrueColorAlpha( gdImageRed(   this->im, this->nColor ),
												   gdImageGreen( this->im, this->nColor ),
												   gdImageBlue(  this->im, this->nColor ),
												   127 - ( ( 127 * nPixelOpacity ) / 16 ) );
				
				// draw the pixel
				gdImageSetPixel( this->im, nX, nY, nDrawColor );
			}
		}
	}
	long lEndTime = clock();	
	std::cout << "[DEBUG] Took " << double( lEndTime - lStartTime ) * 1000 / CLOCKS_PER_SEC << " ms to finishEquation\n";
}
