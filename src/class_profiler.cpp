/***************************************************************************
 *   Copyright (C) 2005-2006 by Ryan Lothian                               *
 *   Unauthorised reproduction of this program is prohibited               *
 *   plotgraph <at> ryanlothian.com                                          *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 ***************************************************************************/

#include "class_profiler.h"

// static variable initialisation
FILE* CProfiler::fLogFile = NULL;

void CProfiler::setLogFile( FILE *fLogSet )
{
	CProfiler::fLogFile = fLogSet;
}
		
void CProfiler::logMessage( const std::string &strMessage )
{
	if ( fLogFile )
	{
		// write message to file
		fputs( strMessage.c_str(), fLogFile );
		fputc( '\n', fLogFile );
	}
	else
	{
		//std::cout << "Profiler: " << strMessage << "\n";
	}
}
		
CProfiler::CProfiler( const std::string &strCallerSet )  // strCallerSet: the function-name of the caller
{
	this->strCallerName = strCallerSet;
	this->logMessage( strCallerName );
}

CProfiler::~CProfiler()
{
	this->logMessage( "~" + strCallerName );
}

