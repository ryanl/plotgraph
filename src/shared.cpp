/***************************************************************************
 *   Copyright (C) 2005-2006 by Ryan Lothian                               *
 *   Unauthorised reproduction of this program is prohibited               *
 *   plotgraph <at> ryanlothian.com                                          *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 ***************************************************************************/

#include "shared.h"

// generates integer between two values (inclusive) chosen randomly
int randomInt( int nMinimum, int nMaximum )
{
	if ( nMaximum > nMinimum )
	{	
		int nRange = nMaximum - nMinimum + 1;
		return nMinimum + int( ( double( nRange ) * rand() ) / ( RAND_MAX + 1.0 ) );
	}
	else // cope with minimum and maximum parameters being given in the wrong order
	{
		int nRange = nMinimum - nMaximum  + 1;
		return nMaximum + int( ( double( nRange ) * rand() ) / ( RAND_MAX + 1.0 ) );
	}
}

// seed the C pseudorandom number generator - rand()
void seedRandom()
{
	// try to use the unix pseudorandom-data device for randomness
	FILE *fRandom = fopen( "/dev/urandom", "r" );
	
	unsigned int nSeed = 0;
		
	if (!fRandom)
	{
		// if we couldn't use /dev/urandom, for example if it is not supported the operating system
		std::cout << "[DEBUG] Could not open /dev/urandom - using time instead\n";
		nSeed = time(0); //then base the seed upon the current time
	}
	else
	{
		// read 1 integer from /dev/urandom to use as the seed
		fread( &nSeed, sizeof(int), 1, fRandom );
  		fclose( fRandom );
	}
	srand( nSeed ); // seed the random number device with our chosen value
}


std::vector< std::string > splitString( const std::string &strText, char cDelimiter )
{
	std::vector< std::string > vstrSplit;
	
	unsigned int nStartCharacter = 0;
	for ( unsigned int nCharacter = 0; nCharacter < strText.length(); nCharacter++ )
	{
		if ( strText.substr( nCharacter, 1 ).c_str()[ 0 ] == cDelimiter )
		{
			std::string strTerm = strText.substr( nStartCharacter, nCharacter - nStartCharacter );
			vstrSplit.push_back( strTerm ); // add part to array
			nStartCharacter = nCharacter + 1;
		}
	}
	std::string strTerm = strText.substr( nStartCharacter, strText.length() - nStartCharacter );
	
	// add final part to array
	vstrSplit.push_back( strTerm );
	
	return vstrSplit;
}
