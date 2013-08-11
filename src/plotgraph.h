/***************************************************************************
 *   Copyright (C) 2005-2006 by Ryan Lothian                               *
 *   Unauthorised reproduction of this program is prohibited               *
 *   ryan <at> ronline.no-ip.info                                          *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 ***************************************************************************/
 
#ifndef INC_gdplotgraph
#define INC_gdplotgraph

#include "sharedmath.h"

#include "class_axis.h"
#include "class_equation.h"

#include <fstream> //for line-based file access
#include <map> //for map data-type

void displayUsageGuide();

class CGraph
{
	private:
		static const DRAWINGMETHODS nDrawingMethod = LINEMETHOD_SUPERSAMPLING; // member constant, sets antialiasing method
		
		gdImagePtr im; //pointer to the image data
		rect rectBound;
		CAxisPair *axes;
		std::map< std::string, CAxis* > axisMap;
		
	public:
		
		CGraph( unsigned int nImageDimensions );
		~CGraph();
				
		void saveImage( const std::string &strSavePath );
		
		void runScript							( std::ifstream &fInput );
		void runScript							( const std::string &strInput );
		void runScriptLine						( const std::string &strLine );
		
		void runScriptLine_axis					( const std::vector< std::string > &v_strSplitLine );
		void runScriptLine_fill					( const std::vector< std::string > &v_strSplitLine );
		void runScriptLine_axispair_cartesian	( const std::vector< std::string > &v_strSplitLine );
		void runScriptLine_axispair_polar		( const std::vector< std::string > &v_strSplitLine );
		void runScriptLine_equationplot			( const std::vector< std::string > &v_strSplitLine );
		void runScriptLine_drawaxes				( const std::vector< std::string > &v_strSplitLine );
		void runScriptLine_axismark				( const std::vector< std::string > &v_strSplitLine );
				
		void fillImage							( int nFillColor );
		void applyWatermark						();
};


#endif /* inc_gdplotgraph */
