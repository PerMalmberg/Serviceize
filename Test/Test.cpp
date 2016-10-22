// Copyright (c) 2016 Per Malmberg
// Licensed under MIT, see LICENSE file.
// Give credit where credit is due.

#include "Catch/include/catch.hpp"
#include <string>
#include <vector>
#include <chrono>
#include <Serviceize\Process.h>

using namespace std::chrono_literals;

SCENARIO( "Can run test project with arguments" )
{
	GIVEN( "A process object" )
	{
		Process proc{ 
			Process::GetExecutableFullPath(), 
			std::vector<std::string>{ "sleep2" } 
		};

		THEN( "When executed" )
		{				
			REQUIRE( proc.Execute() );
			REQUIRE( 0 == proc.GetErrorCode() );
			REQUIRE_FALSE( proc.WaitForTermination( 500ms ) );
			REQUIRE( proc.IsStillActive() );
			int exitCode;
			REQUIRE( proc.WaitForTermination( 2s ) );
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
