/***************************************************************************
 *   Copyright (C) 2005-2006 by Ryan Lothian                               *
 *   Unauthorised reproduction of this program is prohibited               *
 *   ryan <at> ronline.no-ip.info                                          *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 ***************************************************************************/

#ifndef INC_class_axis
#define INC_class_axis

#include "class_linedrawer.h"

#include "sharedmath.h" // for maths functions
#include "gd.h"         // gd graphics library

const int     RENDERCONST_DWIDTH  = 256;
const double  DFONTSIZE           = 8.0;
const int     LINETHICKNESS       = 1;

struct rect
{
	int x1, x2, y1, y2;
};

enum DRAWINGMETHODS
{
	LINEMETHOD_SUPERSAMPLING,
	LINEMETHOD_ALIASED
};

/* CAxis
	Class used to represent and scale values on to an axis 
	TESTED: Module test 4
*/

class CAxis
{
	public:
		 
		// get the upper-bound of the axis' range
		inline double getMaximum() { return this->dMaximum; }
		
		// get the lower-bound of the axis' range
		inline double getMinimum() { return this->dMinimum; }
		
		// set the upper-bound of the axis' range
		inline void setMaximum( double dMaxSet ) { this->dMaximum = dMaxSet; }
		
		// set the lower-bound of the axis' range
		inline void setMinimum( double dMinSet ) { this->dMinimum = dMinSet; }
				
		// fetch an array of the labels/markings on the axis
		std::vector< std::pair<double, std::string> > getAxisMarkings();
		
		// mark a value on this axis, so that a label is added when drawAxes is called
		void	addAxisMarking( double dValue, const std::string &strLabel ); // add a label to the axis at a certain point
		
		// convert a value to the fraction of the axis' length that represents its position upon it
		double logicalToFractional( double dInput );
		
		CAxis( double dMinSet, double dMaxSet, double dIntervalSet ) : dMinimum(dMinSet), dMaximum(dMaxSet), dInterval(dIntervalSet)
		{
			std::cout << "[DEBUG] CAxis::CAxis() - " << dMinSet << " to " << dMaxSet << "\n";
		}
		
	private:
		double dMinimum, dMaximum, dInterval;
		std::vector< std::pair<double, std::string> > markings;
};

/* CAxisPair
	Virtual class used to handle a co-ordinate system
	TESTED: Module test 5
*/

class CAxisPair
{
	protected:
		gdImagePtr im;
		CLineDrawer *plotter;
		
		std::pair<double, double> prdXYPrevious;
		
	public:
		virtual std::pair<double, double> convertCoordinateLogicalToReal( double dX, double dY ) = 0;
		
		virtual double getMaximumLogical( int nAxis ) = 0;
		virtual double getMinimumLogical( int nAxis ) = 0;
		
		virtual void drawAxes( int nAxisColor, int nGridColor, int nTextColor ) = 0;
		virtual void applyBoundingBox() = 0;
		
		// create a new CAxisPair ojbect
		CAxisPair( gdImagePtr imset, DRAWINGMETHODS nMethod ) : im(imset)
		{
			switch ( nMethod ) // create the appropriate linedrawer object
			{
				case LINEMETHOD_SUPERSAMPLING:
					plotter = new CLineDrawer_Supersampled( im );
					break;
					
				case LINEMETHOD_ALIASED:
					plotter = new CLineDrawer_Aliased( im );
					break;
					
				default: //invalid LINEMETHOD
					std::cerr << "[FATAL] Unknown line drawing method selected!" << std::endl;
					exit(-1);
			}
		}
		
		virtual ~CAxisPair() { delete plotter; }
		
		// signal the start of the drawing of an equation
		void startEquation( int nColor )
		{
			plotter->startEquation();
			plotter->setLineParameters( nColor );
		}
		
		// signal the end of the drawing of an equation
		void finishEquation()
		{
			plotter->finishEquation();
		}
		
		// plot a single point
		void unconnectedPoint( double dLogicalX, double dLogicalY )
		{
			//std::cout << "[DEBUG] CAxisPair::unconnectedPoint() - (" << dLogicalX << "," << dLogicalY << ")\n";
			std::pair<double, double> prdXY = convertCoordinateLogicalToReal( dLogicalX, dLogicalY );
			plotter->drawLine( prdXY.first, prdXY.second, prdXY.first, prdXY.second ); //optional [KEYWORD]
			prdXYPrevious = prdXY;
		}
		
		// draw a point connected to the last co-ordinate drawn
		void connectPoint( double dLogicalX, double dLogicalY )
		{
			//std::cout << "[DEBUG] CAxisPair::connectPoint() - (" << dLogicalX << "," << dLogicalY << ")\n";
			std::pair<double, double> prdXY = convertCoordinateLogicalToReal( dLogicalX, dLogicalY );
			plotter->drawLine( prdXYPrevious.first, prdXYPrevious.second, prdXY.first, prdXY.second );
			prdXYPrevious = prdXY;
		}
		virtual rect getBoundingBox() = 0;
};

/* CAxisPair_Cartesian
	Class used to handle a Cartesian co-ordinate system
	TESTED: Module test 5
*/

class CAxisPair_Cartesian : public CAxisPair
{
	private:
		CAxis &AxisX, &AxisY;
		rect rectBound;
		
	public:
		// convert a logical (mathematical) co-ordinate to a pixel co-ordinate
		virtual std::pair<double, double> convertCoordinateLogicalToReal( double dX, double dY )
		{
			double dXFractional = AxisX.logicalToFractional( dX );
			double dYFractional = AxisY.logicalToFractional( dY );
			
			double dXReal = rectBound.x1 + ( dXFractional * ( rectBound.x2 - rectBound.x1 ) );
			double dYReal = rectBound.y2 - ( dYFractional * ( rectBound.y2 - rectBound.y1 ) );
			return std::pair<double, double> ( dXReal, dYReal );
		}
		
		// get maximum value of a component axis
		virtual double getMaximumLogical( int nAxis )
		{
			if (nAxis == 1)
				return AxisX.getMaximum();
			else
				return AxisY.getMaximum();
		}
		
		// get minimum value of a component axis
		virtual double getMinimumLogical( int nAxis )
		{
			if (nAxis == 1)
				return AxisX.getMinimum();
			else
				return AxisY.getMinimum();
		}
			
		CAxisPair_Cartesian( gdImagePtr imset, DRAWINGMETHODS nMethod, CAxis &AxisSetX, CAxis &AxisSetY, rect rectBoundSet ) : CAxisPair(imset, nMethod), AxisX(AxisSetX), AxisY(AxisSetY), rectBound(rectBoundSet) {}
	
		// draw the X and Y axes
		void drawAxes( int nAxisColor, int nGridColor, int nTextColor );
		
		// prevent drawing outside the allocated area
		void applyBoundingBox()
		{
			gdImageSetClip( this->im, this->rectBound.x1, this->rectBound.y1, this->rectBound.x2, this->rectBound.y2 );
		}
		inline virtual rect getBoundingBox() { return rectBound; }		
};

/* CAxisPair_Polar
	Class used to handle a polar co-ordinate system
	TESTED: Module test 5
*/

class CAxisPair_Polar : public CAxisPair
{
	private:
		CAxis &AxisR, &AxisTheta;
		rect rectBound;
		
	public:
		
		
		// convert a logical (mathematical) co-ordinate to a pixel co-ordinate
		std::pair<double, double> convertCoordinateLogicalToReal( double dR, double dTheta )
		{
			double dRFractional = AxisR.logicalToFractional( dR );
			
			if ( dRFractional < 0.0 ) //negative R - it is customary to plot such points as if R=0
				dRFractional = 0.0;
			
			double dRReal = dRFractional * ( rectBound.x2 - rectBound.x1 ) / 2;
			
			double dX = rectBound.x1 + ( ( rectBound.x2 - rectBound.x1 ) / 2.0 ) + ( dRReal * cos( dTheta ) );
			double dY = rectBound.y2 - ( ( rectBound.y2 - rectBound.y1 ) / 2.0 ) - ( dRReal * sin( dTheta ) );
			
			return std::pair<double, double> ( dX, dY );
		}
		
		// get maximum value of a component axis
		virtual double getMaximumLogical( int nAxis )
		{
			if (nAxis == 1)
				return AxisR.getMaximum();
			else
				return AxisTheta.getMaximum();
		}
		
		// get minimum value of a component axis
		virtual double getMinimumLogical( int nAxis )
		{
			if (nAxis == 1)
				return AxisR.getMinimum();
			else
				return AxisTheta.getMinimum();
		}
		CAxisPair_Polar( gdImagePtr imset, DRAWINGMETHODS nMethod, CAxis &AxisSetR, CAxis &AxisSetTheta, rect rectBoundSet ) : CAxisPair(imset, nMethod), AxisR(AxisSetR), AxisTheta(AxisSetTheta), rectBound(rectBoundSet) {}
		
		// draw the R and THETA axes
		void drawAxes( int nAxisColor, int nGridColor, int nTextColor );
		
		// prevent drawing outside the allocated area
		void applyBoundingBox()
		{
			gdImageSetClip( this->im, rectBound.x1, rectBound.y1, rectBound.x2, rectBound.y2 );
		}
		inline virtual rect getBoundingBox() { return rectBound; }
};

#endif /* INC_class_axis */
