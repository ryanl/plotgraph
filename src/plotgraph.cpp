/***************************************************************************
 *   Copyright (C) 2005-2006 by Ryan Lothian                               *
 *   Unauthorised reproduction of this program is prohibited               *
 *   plotgraph <at> ryanlothian.com                                          *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 ***************************************************************************/

#include "plotgraph.h"

// output a few lines of help to the console, explaining the usage of the program
void displayUsageGuide()
{
	std::cout << "Usage: gdplotgraph {outputimage} {renderwidth} {inputscript}" << std::endl;
}

// apply a watermark to the graph
void CGraph::applyWatermark()
{
	std::cout << "Applying watermark\n";
	
	int nBrect[8], nWatermarkColor = gdTrueColorAlpha( 0, 0, 40, 90 );
	
	gdImageSetClip( im, 0, 0, im->sx - 1, im->sy - 1 );
	gdImageStringFT( im, nBrect, nWatermarkColor, "/usr/share/fonts/bitstream-vera/VeraIt.ttf", 4.0 + ( im->sx / 64.0 ), 0.0, int( im->sx / 16.0 ), int(im->sx * 30.0 / 32.0),
		"www.ryanlothian.com" );
}

// output the image as a PNG file
void CGraph::saveImage( const std::string &strSavePath )
{
	//std::cout << "Saved graph as \"" << strSavePath << "\"\n";	
	
	FILE *pngout = fopen( strSavePath.c_str() , "wb" ); // Image output -> PNG file

	/* Output the image to the disk file in PNG format. */
	
	gdImagePngEx( im, pngout, 7 );
	fclose( pngout );
}

// deconstructor - deallocate resources
CGraph::~CGraph()
{
	gdImageDestroy( im ); //pointer to the image data
	//delete the current axispair object if it exists
	delete axes;
	
	std::map< std::string, CAxis* >::iterator itErase;
	for ( itErase = axisMap.begin(); itErase != axisMap.end(); itErase++ )
	{
		 //delete each axis that has been created
		delete itErase->second;
	}
}

// constructor - create graph image
CGraph::CGraph( unsigned int nImageDimensions )
{
	if ( nImageDimensions < 10 ) nImageDimensions = 10;
	if ( nImageDimensions > 2048 ) nImageDimensions = 2048;
	
	this->im = gdImageCreateTrueColor( nImageDimensions, nImageDimensions );  // Allocate memory for the output image
	rect rectBoundSet = { 0, im->sx, 0,  im->sy };
	rectBound = rectBoundSet;
	
	gdImageSetThickness( im, 1 ); //unit-width lines where applicable
	
	axes = NULL;
}

// fill the image completely with a single colour
void CGraph::fillImage( int nFillColor )
{
	gdImageFilledRectangle( this->im, 0, 0, im->sx - 1, im->sy - 1, nFillColor ); //fill the entire image with a colour
}

// get the first word (all the text before the first space) of a string
inline std::string getFirstWord( const std::string &strLine )
{
	unsigned int nChar;
	for ( nChar = 0; nChar < strLine.length(); nChar++ )
	{
		if ( strLine.substr( nChar, 1 ) == " " ) break;
	}
	return strLine.substr( 0, nChar );
}

void CGraph::runScriptLine_axis( const std::vector< std::string > &v_strSplitLine )
{
	std::string strAxisName = v_strSplitLine.at( 1 );
	if ( axisMap[strAxisName] )
		std::cout << "[ERROR] You cannot alter an existing axis - create a new one instead." << std::endl;
	else
	{
		double dMin				= atof( v_strSplitLine.at( 2 ).c_str() );
		double dMax				= atof( v_strSplitLine.at( 3 ).c_str() );
		double dMarkingInterval = 0.0;
				
		if ( v_strSplitLine.size() >= 5 ) dMarkingInterval = atof( v_strSplitLine.at( 4 ).c_str() );
				
		if ( v_strSplitLine.at( 0 ) == "axispi" ) //values were given in terms of pi
		{
			dMin *= MATHCONST_PI;
			dMax *= MATHCONST_PI;
			dMarkingInterval *= MATHCONST_PI;
		}
				
		if ( dMarkingInterval != 0.0 && fabs((dMax - dMin) / dMarkingInterval) > 100.0 )
		{
			std::cout << "[ERROR] Too many marks specified on axis therefore no marks will be drawn.\n";
			dMarkingInterval = 0.0;
			//if over 100 marks would be drawn, this would serve little purpose and take some time to do so don't draw any marks
		}
				
		CAxis *axisNew = new CAxis( dMin, dMax, dMarkingInterval );
		this->axisMap[strAxisName] = axisNew;
	}
}

void CGraph::runScriptLine_axismark( const std::vector< std::string > &v_strSplitLine )
{
	const std::string &strAxisName = v_strSplitLine.at( 1 );
	CAxis *axisMark = axisMap[strAxisName];
			
	if ( !axisMark )
		std::cout << "[ERROR] Axis mistyped or nonexistent - create it first!\n";
	else
	{
		// allow an unlimited number of mark position parameters
		for ( unsigned int nSplitIndex = 2; nSplitIndex < v_strSplitLine.size(); nSplitIndex++ )
		{
			const std::string &strMarkValue = v_strSplitLine.at( nSplitIndex );
			double dMarkValue = atof( strMarkValue.c_str() );
			axisMark->addAxisMarking( dMarkValue, strMarkValue );
		}
	}
}

void CGraph::runScriptLine_fill( const std::vector< std::string > &v_strSplitLine )
{
	int nBgColor = 0, nColorUnrecognised = 0;
	if ( v_strSplitLine.size() == 2 ) // named colour
	{
		const std::string &strColorName = v_strSplitLine.at( 1 );
		
		if      ( strColorName == "white"	)	nBgColor = gdTrueColor( 255, 255, 255 );
		else if ( strColorName == "black"	)	nBgColor = gdTrueColor( 0, 0, 0 );
		else if ( strColorName == "red"		)	nBgColor = gdTrueColor( 255, 0, 0 );
		else if ( strColorName == "green"	)	nBgColor = gdTrueColor( 0, 255, 0 );
		else if ( strColorName == "blue"	)	nBgColor = gdTrueColor( 0, 0, 255 );
		else
		{
			nColorUnrecognised = 1;
			std::cout << "[ERROR] Fill colour unrecognised\n";
		}
	}
	else if ( v_strSplitLine.size() == 4 ) // given as RGB byte values
	{
		int nColorComponents[3] = { atoi( v_strSplitLine.at( 1 ).c_str() ),
			                        atoi( v_strSplitLine.at( 2 ).c_str() ),
									atoi( v_strSplitLine.at( 3 ).c_str() )
		                          };
		
		for ( unsigned int nComponent = 0; nComponent < 3; nComponent++ )
		{
			// range check validation
			if ( nColorComponents[nComponent] > 255 ) nColorComponents[nComponent] = 255;
			if ( nColorComponents[nComponent] < 0 )   nColorComponents[nComponent] = 0;
		}
		
		nBgColor = gdTrueColor( nColorComponents[0], nColorComponents[1], nColorComponents[2] );
	}
	else
	{
		std::cout << "[ERROR] Wrong number of arguments to 'fill'\n";
		nColorUnrecognised = 1;
	}
	
	if ( !nColorUnrecognised )
		this->fillImage( nBgColor );
}

void CGraph::runScriptLine_axispair_cartesian( const std::vector< std::string > &v_strSplitLine )
{
	if ( this->axes ) // destroy existing axispair
	{
		delete this->axes;
		this->axes = NULL;
	}
			
	std::string strAxisXName = v_strSplitLine.at( 1 );
	std::string strAxisYName = v_strSplitLine.at( 2 );
			
	CAxis *axisX = axisMap[strAxisXName];
	CAxis *axisY = axisMap[strAxisYName];
			
	if ( !axisX )
		std::cout << "[ERROR] X-axis mistyped or nonexistent - create it first!\n";
	else if ( !axisY )
		std::cout << "[ERROR] Y-axis mistyped or nonexistent - create it first!\n";
	else
	{
		this->axes = new CAxisPair_Cartesian( this->im, nDrawingMethod, *axisX, *axisY, this->rectBound );
	}
}

void CGraph::runScriptLine_axispair_polar( const std::vector< std::string > &v_strSplitLine )
{
	if ( this->axes ) //destroy existing axispair
	{
		delete this->axes;
		this->axes = NULL;	
	}
				
	std::string strAxisRName = v_strSplitLine.at( 1 );
	std::string strAxisThetaName = v_strSplitLine.at( 2 );
		
	CAxis *axisR = axisMap[strAxisRName];
	CAxis *axisTheta = axisMap[strAxisThetaName];
		
	if ( !axisR )
		std::cout << "[ERROR] R-axis mistyped or nonexistent - create it first!" << std::endl;
	else if ( !axisTheta )
		std::cout << "[ERROR] THETA-axis mistyped or nonexistent - create it first!" << std::endl;
	else
	{
		this->axes = new CAxisPair_Polar( this->im, nDrawingMethod, *axisR, *axisTheta, this->rectBound );
	}
}

void CGraph::runScriptLine_equationplot( const std::vector< std::string > &v_strSplitLine )
{
	if ( !axes )
		std::cout << "[ERROR] You must setup your axes before plotting an equation" << std::endl;
	else
	{
		int nLinecolor = gdTrueColor( randomInt( 0, 255 ), randomInt( 0, 255 ), randomInt( 0, 255 ) ); // allocate line color (randomly chosen)
		int nSamples = 1500;
					
		const std::string &strEquation = v_strSplitLine.at( 1 );
		if ( v_strSplitLine.size() >= 3 ) nSamples = atoi( v_strSplitLine.at( 2 ).c_str() );
		
		// range check on nSamples
		if ( nSamples < 2 )    nSamples = 2;
		if ( nSamples > 5000 ) nSamples = 5000;
		
		CEquation CurrentEquation( strEquation, nLinecolor );
		if ( !CurrentEquation.checkSyntax() )
			std::cout << "[ERROR] Syntax error(s) in your equation (" << strEquation << ")" << std::endl;
		else
			CurrentEquation.plot( im, *axes, nSamples );
	}
}

void CGraph::runScriptLine_drawaxes( const std::vector< std::string > &v_strSplitLine )
{
	if ( !axes )
		std::cout << "[ERROR] You must setup your axes before drawing them" << std::endl;	
	else
	{
		int nForecolor = gdTrueColor( 20,  40,  200 ); //blue
		int nAxisDark  = gdTrueColor( 120, 120, 120 ); //dark grey
		int nAxisGrey  = gdTrueColor( 220, 220, 220 ); //grey
		
		this->axes->drawAxes( nAxisDark, nAxisGrey, nForecolor );
	}
}

// perform the a graph-plotting script instruction
void CGraph::runScriptLine( const std::string &strLine )
{
	std::cout << "[INFO] Processing instruction: " << strLine << "\n";
	
	std::vector< std::string > v_strSplitLine = splitString( strLine, ' ' );
	if ( v_strSplitLine.size() == 0 || v_strSplitLine.at( 0 ) == "" ) return;
		
	std::string &strCommand = v_strSplitLine.at( 0 );
		
	if ( ( strCommand == "axis" || strCommand == "axispi" ) && v_strSplitLine.size() >= 4)
	{
		this->runScriptLine_axis( v_strSplitLine );
	}
	else if ( strCommand == "axismark"  && v_strSplitLine.size() >= 3 )
	{
		this->runScriptLine_axismark( v_strSplitLine );
	}
	else if ( strCommand == "fill" )
	{
		this->runScriptLine_fill( v_strSplitLine );
	}
	else if ( strCommand == "axispair_cartesian" && v_strSplitLine.size() >= 3 )
	{
		this->runScriptLine_axispair_cartesian( v_strSplitLine );
	}
	else if ( strCommand == "axispair_polar" && v_strSplitLine.size() >= 3 )
	{
		this->runScriptLine_axispair_polar( v_strSplitLine );
	}		
	else if ( strCommand == "equationplot" && v_strSplitLine.size() >= 2 )
	{
		this->runScriptLine_equationplot( v_strSplitLine );
	}
	else if ( strCommand == "drawaxes" )
	{
		this->runScriptLine_drawaxes( v_strSplitLine );
	}
	else if ( strCommand == "end" )
	{
		// do nothing - if necessary, this will be handed by CGraph::runScript later
	}
	else
	{
		std::cout << "[ERROR] Unrecognised command '" << strCommand << "'" << std::endl;
	}
}

// Execute a list of graph plotting instructions, one per line
// using a string as input
void CGraph::runScript( const std::string &strScript )
{
	// split the script in to an array of lines
	std::vector<std::string> v_strLines = splitString( strScript, '\n' );
	for ( unsigned int nLine = 0; nLine < v_strLines.size(); nLine++ )
	{
		std::string &strLine = v_strLines.at( nLine );
		this->runScriptLine( strLine );
	}
}

// Execute a list of graph plotting instructions, one per line
//using a file as input
void CGraph::runScript( std::ifstream &fInput )
{
	std::string strLine;
	
	while ( ! (fInput.bad() || fInput.eof() || strLine == "end" ) )
	{
		// fetch and run each line sequentially
		getline( fInput, strLine );
		this->runScriptLine(strLine); 
	}
}

// Execution entrance point
int main( int argc, const char* argv[] )
{	
	seedRandom();
	
	// display version and copyright information
	std::cout << "gdPlotGraph - v1.0.1" << std::endl << "Copyright (C) 2005-2006 Ryan Lothian" << std::endl;
	
	// open a file for logging, creating it if necessary
	FILE *fLog = NULL; //fopen( "profilerlog.txt", "w" );
	if ( fLog )
		CProfiler::setLogFile( fLog );
	else
	{
		//std::cout << "Could not open profilerlog.txt for writing" << std::endl;
	}
	
	if ( argc < 4 ) // validation - check whether not enough arguments given
	{
		displayUsageGuide(); // if the program hasn't been supplied with appropriate parameters, display basic help
	}
	else
	{
		long lStartTime = clock();		
	
		CProfiler functionprofile( "main" );
		
		std::string strOutputFile 	= std::string( argv[1] );
		unsigned int nRenderWidth	= atoi( argv[2] );
		std::string strInputFile	= std::string( argv[3] );
		
		CGraph graph( nRenderWidth );
		
		std::string strScript;
		std::ifstream fInput;
		fInput.open( strInputFile.c_str() );
		graph.runScript( fInput );
		graph.applyWatermark(); // mark the image with the text "gdplotgraph@ronline.no-ip.info" to indicate its origin
		
		long lEndTime = clock();		
		std::cout << "[DEBUG] Took " << double( lEndTime - lStartTime ) * 1000 / CLOCKS_PER_SEC << " ms to render graph\n";
		
		graph.saveImage( strOutputFile ); // and write it to disk as a PNG file
		
		lEndTime = clock();		
		std::cout << "[DEBUG] Took " << double( lEndTime - lStartTime ) * 1000 / CLOCKS_PER_SEC << " ms to render and then save graph\n";
	}
	
	if ( fLog )
	{
		CProfiler::setLogFile( NULL );
		fclose( fLog );
	}
	
	return 0;
}
