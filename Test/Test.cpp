// Copyright (c) 2016 Per Malmberg
// Licensed under MIT, see LICENSE file.
// Give credit where credit is due.

#include "Catch/include/catch.hpp"
#include <string>
#include <vector>
#include <chrono>
#include <Serviceize\Process.h>

using namespace std::chrono_literals;

namespace std {
template<>
class default_delete< HANDLE >
{
public:
	void operator()( HANDLE& h )
	{
		CloseHandle( h );
	}
};
}


SCENARIO( "Can run test project with arguments" )
{
	GIVEN( "A process object" )
	{
		Process proc{ 
			"d:\\git\\Serviceize\\Test\\dist\\bin\\Debug\\Test.exe", 
			std::vector<std::string>{ "return2" } 
		};

		THEN( "When executed" )
		{				
			REQUIRE( proc.Execute() );
			REQUIRE( 0 == proc.GetErrorCode() );
			REQUIRE( proc.WaitForTermination( 1s ) );
			int exitCode;
			REQUIRE( proc.GetExitCode( exitCode ) );
			REQUIRE( exitCode == 2 );
		}
	}
}

SCENARIO( "File to execute doesn't exist" )
{
	GIVEN( "A process object" )
	{
		Process proc{ "FileDoesNotExist" };

		THEN( "When executed" )
		{
			REQUIRE_FALSE( proc.Execute() );
			REQUIRE( proc.GetErrorCode() == ERROR_FILE_NOT_FOUND );
		}
		
	}
}
