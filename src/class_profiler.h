/***************************************************************************
 *   Copyright (C) 2005-2006 by Ryan Lothian                               *
 *   Unauthorised reproduction of this program is prohibited               *
 *   ryan <at> ronline.no-ip.info                                          *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 ***************************************************************************/

// Program profiling class
// used for timing times required for function calls and logging what functions are called

#include <cstdio>
#include <string>
#include <iostream>

/* CProfiler
	Class used to aid debugging by logging function calls
	TESTED: Module test 1
*/

class CProfiler
{
	private:
		std::string strCallerName; // the name of the function that created this profiler object
		
	public:
		static FILE* fLogFile; //the output log file for all profilers
		static void setLogFile( FILE* fLogSet );
		
		void logMessage( const std::string &strMessage );
		
		CProfiler( const std::string &strCallerSet ); // strCallerSet: the name of the caller function
		~CProfiler();
};
