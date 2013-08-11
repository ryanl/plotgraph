/***************************************************************************
 *   Copyright (C) 2005-2006 by Ryan Lothian                               *
 *   Unauthorised reproduction of this program is prohibited               *
 *   ryan <at> ronline.no-ip.info                                          *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 ***************************************************************************/

#ifndef INC_shared
#define INC_shared

#include <stdlib.h>
#include <fstream>		// for filehandling
#include <iostream>		// for console I/O
#include <stdio.h>
#include <dirent.h> 	// for directory contents listing
#include <string>		// for string
#include <ctime>		// for time functions, e.g. to seed rand

#include <list>			// for lists, arrays
#include <vector>		// for lists, arrays


#include "class_profiler.h" // used widely throughout the program for debugging and performance analysis

#define globalFONTPATH "/usr/share/fonts/truetype/ttf-bitstream-vera/Vera.ttf" // path to the font we will use for axis labels
const int globalFONTSIZE = 11;

/* splitString()
	Splits a string by a certain separator character into an array of substrings
	TESTED: Unit test 1
*/
std::vector<std::string> splitString( const std::string &strText, char cDelimiter );


/* seedRandom()
	Generate a random integer between a given minimum and maximum inclusive
	TESTED: Unit test 3
*/
int randomInt( int nMinimum, int nMaximum );

/* seedRandom()
	Initialise the random number generator
	TESTED: Unit test 3
*/
void seedRandom();

/* swapv()
	Swap the contents/value of two variables regardless of their type
	TESTED: Unit test 2
*/
template< class TSwap > inline void swapv( TSwap& nA, TSwap& nB ) { TSwap nTemp = nB; nB = nA; nA = nTemp; }

/* isNumeric()
	Returns true for numeric characters, and false for non-numeric characters
	TESTED: Unit test 5
*/
inline bool isNumeric( char cDigit )
{
	if ( cDigit >= '0' && cDigit <= '9' ) return true; else return false;
}


inline std::string doubleToFixedString( double dValue )
{
	char szValueText[32] = "";
	snprintf( szValueText, 31, "%f", dValue );
	return std::string( szValueText );
}

inline std::string doubleToString( double dValue )
{
	char szValueText[32] = "";
	snprintf( szValueText, 31, "%g", dValue );
	return std::string( szValueText );
}


#endif /* INC_shared */
