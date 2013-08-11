/***************************************************************************
 *   Copyright (C) 2005-2006 by Ryan Lothian                               *
 *   Unauthorised reproduction of this program is prohibited               *
 *   ryan <at> ronline.no-ip.info                                          *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 ***************************************************************************/

#ifndef INC_class_linedrawer
#define INC_class_linedrawer

#include "gd.h"
#include <iostream>
#include "shared.h"
#include "sharedmath.h"

inline int intpart(double dX) { return int(dX); }
inline int intround(double dX) { return int(dX + 0.5); }
inline double fractpart(double dX) { return dX - intpart(dX); }
inline double revfractpart(double dX) { return 1.0f - fractpart(dX); }


/* CLineDrawer, CLineDrawer_Aliased, CLineDrawer_Supersampled
	Class used to draw line segments
	TESTED: Module test 3
*/

class CLineDrawer /* virtual class - use a specific subclass instead */
{
	protected:
		int nColor;
		gdImagePtr im;
		
	public:
		CLineDrawer( gdImagePtr imset ) : im(imset) {}
		virtual ~CLineDrawer() {};
		virtual void finishEquation() = 0; //an equation has been completely plotted
		
		void setLineParameters( int nSetColor ) { this->nColor = nSetColor; }
		
		virtual void startEquation() = 0;
		
		virtual void drawLine( double dX1, double dY1, double dX2, double dY2 ) = 0;
};

class CLineDrawer_Aliased : public CLineDrawer
{
	public:
		CLineDrawer_Aliased( gdImagePtr imset ) : CLineDrawer(imset) {}
		~CLineDrawer_Aliased() {}
		
		virtual void finishEquation() {}
		virtual void startEquation()  {}
		
		virtual void drawLine( double dX1, double dY1, double dX2, double dY2 )
		{
			std::cout << "[DEBUG] CLineDrawer_Aliased::drawLine() - (" << dX1 << ","
					<< dY1 << ") to (" << dX2 << "," << dY2 << ")\n";
			gdImageLine(this->im, intround(dX1), intround(dY1), intround(dX2), intround(dY2), this->nColor);
		}
};

class CLineDrawer_Supersampled : public CLineDrawer
{
	private:
		unsigned int nMultiplier;
		unsigned short *nSuperPixels;
		unsigned int nBitsSetLookupTable[65536];

		inline void plotPixel( int nX, int nY )
		{
			if ( nX >= 0 && nX < ( im->sx << 2 ) && nY >= 0 && nY < ( im->sy << 2 ) )
			{
				// calculate the element of the array that the pixel resides in
				unsigned int nSuperPixelIndex = ( ( nX >> 2 ) * im->sy ) + ( nY >> 2 );
				// calculate the index of the bit that must be set
				unsigned int nBit = (  (nX & 3 ) << 2 ) + ( nY & 3 );
				this->nSuperPixels[ nSuperPixelIndex ] |= ( 1 << nBit ); //mark bit
			}
		}
		
	public:
		CLineDrawer_Supersampled( gdImagePtr imset ) : CLineDrawer(imset)
		{
			const unsigned short nUnity = 1;
			this->nMultiplier = 4;
			
			this->nSuperPixels = new unsigned short[im->sx * im->sy];
			if ( !nSuperPixels )
			{
				std::cout << "[FATAL] Could not allocate memory for supersampling pixel array!\n";
				exit(-1);
			}
			for ( unsigned int nBitPattern = 0; nBitPattern < 65536; nBitPattern++ )
			{
				unsigned int nPixelOpacity = 0;
				for ( unsigned short nBit = 0; nBit < 16; ++nBit )
				{
					nPixelOpacity += ( nBitPattern >> nBit ) & nUnity;  //erase pixel array
				}
				nBitsSetLookupTable[ nBitPattern ] = nPixelOpacity;
			}
		}
		virtual ~CLineDrawer_Supersampled() {}
				
		virtual void finishEquation();
		virtual void startEquation();
		virtual void drawLine( double dX1, double dY1, double dX2, double dY2 );
};

#endif /* INC_class_linedrawer */
